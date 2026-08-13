/**************************************************************************/
/*  gcl_script.cpp                                                        */
/**************************************************************************/
/*  GCL registered as a Godot ScriptLanguage.                             */
/*  Extensions: .gcsf (script), .gclib (library)                          */
/*  Dil icerigi:                                                          */
/*    #            -> satir yorumu                                        */
/*    #| ... |#    -> blok yorumu                                         */
/*    @extern <node> -> self.<node> erisimi (otomatik tamamlama + calisma) */
/*    <tip> <isim>  -> degisken tanimi                                  */
/*    void Ready() / Update() / PhysicsUpdate() -> isletim fonksiyonlari  */
/**************************************************************************/

#include "gcl_script.h"
#include "core/class_db.h"
#include "core/engine.h"
#include "core/object.h"
#include "Executor/executor.h"
#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "scene/main/node.h"

namespace gcl {

GCLScriptLanguage *GCLScriptLanguage::singleton = nullptr;

void GCLScript::_bind_methods() {
}

ScriptLanguage *GCLScript::get_language() const {
	return GCLScriptLanguage::get_singleton();
}

GCLScriptInstance::GCLScriptInstance(Object *p_owner, Ref<GCLScript> p_script) {
	owner = p_owner;
	script = p_script;
	load_members();

	/* Update / UpdatePhysics her kare cagrilsin diye islemeyi ac.
	   Editor'de ACILMAZ: yoksa oyun durdurulsa bile editor sahnesinde
	   Update her kare calisip console'a cikti basmaya devam eder. */
	if (owner && !Engine::get_singleton()->is_editor_hint()) {
		owner->call("set_process", Variant(true));
		owner->call("set_physics_process", Variant(true));
	}
}

ScriptInstance *GCLScript::instance_create(Object *p_this) {
	GCLScriptInstance *instance = memnew(GCLScriptInstance(p_this, Ref<GCLScript>(this)));
	instances.insert(p_this);
	return instance;
}

/* ------------------------------------------------------------------ */
/*  GCLScriptInstance - self sistemi                                   */
/* ------------------------------------------------------------------ */

void GCLScriptInstance::load_members() {
	members.clear();

	String code = script->get_source_code();

	/* @extern <node> -> members["<node>"] (Node yolu olarak tutulur) */
	Vector<String> nodes;
	extern_scan(code, nodes);
	for (int i = 0; i < nodes.size(); i++) {
		members[StringName(nodes[i])] = Variant();
	}

	/* <tip> <isim> degiskenleri -> members["<isim>"] */
	Vector<String> variables;
	variable_scan(code, variables);
	for (int i = 0; i < variables.size(); i++) {
		members[StringName(variables[i])] = Variant();
	}
}

bool GCLScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	if (members.has(p_name)) {
		members[p_name] = p_value;
		return true;
	}
	return false;
}

bool GCLScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	if (members.has(p_name)) {
		r_ret = members[p_name];
		return true;
	}
	return false;
}

void GCLScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
	for (const Map<StringName, Variant>::Element *E = members.front(); E; E = E->next()) {
		p_properties->push_back(PropertyInfo(Variant::NIL, E->key()));
	}
}

Variant::Type GCLScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	if (r_is_valid) {
		*r_is_valid = members.has(p_name);
	}
	return Variant::NIL;
}

void GCLScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
	p_list->push_back(MethodInfo("Ready"));
	p_list->push_back(MethodInfo("Update", PropertyInfo(Variant::REAL, "delta")));
	p_list->push_back(MethodInfo("UpdatePhysics", PropertyInfo(Variant::REAL, "delta")));
}

bool GCLScriptInstance::has_method(const StringName &p_method) const {
	return p_method == "Ready" || p_method == "Update" || p_method == "UpdatePhysics";
}

/* Godot bildirimlerini GCL fonksiyonlarina baglar:
   READY -> Ready(), PROCESS -> Update(delta), PHYSICS_PROCESS -> UpdatePhysics(delta).
   Editor'de (oyun kapaliyken) hicbir fonksiyon tetiklenmez: yoksa Update her kare
   calisip console'a cikti basmaya devam eder. */
void GCLScriptInstance::notification(int p_notification) {
	/* Editor modunda calismasini ONLE (cift guvenlik: process kapali olsa bile). */
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	if (p_notification == Node::NOTIFICATION_READY) {
		Variant::CallError err;
		call("Ready", nullptr, 0, err);
	} else if (p_notification == Node::NOTIFICATION_PROCESS) {
		Variant::CallError err;
		call("Update", nullptr, 0, err);
	} else if (p_notification == Node::NOTIFICATION_PHYSICS_PROCESS) {
		Variant::CallError err;
		call("UpdatePhysics", nullptr, 0, err);
	}
}

Variant GCLScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	if (p_method == "Ready" || p_method == "Update" || p_method == "UpdatePhysics") {
		String code = script->get_source_code();

		/* Kullanici fonksiyonlari ve class'lar kaynak kodda aranir. */
		types.source = code;
		types.call_depth = 0;

		/* Global scope atamalar (<tip> <isim> = ...) SADECE Ready'de islenir.
		   Update/UpdatePhysics her kare cagrildigi icin global atamalarin
		   tekrarlanmasi counter/sum'u sifirlar ve CHILD("ali") gibi instance
		   kurulumlari her frame'de yeniden calisir. */
		if (p_method == "Ready") {
			String global_code = executor_strip_bodies(code);
			executor_run(global_code, members, types);
		}

		/* Cagrilan fonksiyonun govdesi bulunur ve yorumlanir. */
		String body;
		if (executor_find_body(code, p_method, body)) {
			executor_run(body, members, types);
		}

		r_error.error = Variant::CallError::CALL_OK;
		return Variant();
	}
	r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
	return Variant();
}

/* ------------------------------------------------------------------ */
/*  ScriptLanguage                                                     */
/* ------------------------------------------------------------------ */

Error GCLScriptLanguage::execute_file(const String &p_path) {
	return OK;
}

void GCLScriptLanguage::get_reserved_words(List<String> *p_words) const {
	/* Dildeki anahtar kelimeler. */
	p_words->push_back("if");
	p_words->push_back("else");
	p_words->push_back("while");
	p_words->push_back("for");
	p_words->push_back("foreach");
	p_words->push_back("return");
	p_words->push_back("void");
	p_words->push_back("self");
	p_words->push_back("@extern");
}

bool GCLScriptLanguage::is_control_flow_keyword(String p_string) const {
	return p_string == "if" || p_string == "else" || p_string == "while" ||
			p_string == "for" || p_string == "foreach" || p_string == "return";
}

Ref<Script> GCLScriptLanguage::get_template(const String &p_class_name, const String &p_base_class_name) const {
	Ref<GCLScript> s;
	s.instance();

	/* Node secilip script olusturuldugunda editor secili node'un tipini
	   (sinif adini) p_base_class_name olarak gecer. @extern satirina o tip yazilir. */
	String code = "@extern " + p_base_class_name + "\n\n";
	code += "void Ready() {\n\t\n}\n\n";
	code += "void Update(float delta) {\n\t\n}\n\n";
	code += "void UpdatePhysics(float delta) {\n\t\n}\n";

	s->set_source_code(code);
	return s;
}

Script *GCLScriptLanguage::create_script() const {
	return memnew(GCLScript);
}

bool GCLScriptLanguage::validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path, List<String> *r_functions, List<Warning> *r_warnings, Set<int> *r_safe_lines) const {
	/* Yorumları ayıkla — bu her girdide çalışır. */
	gcl::strip_comments(p_script);
	return true;
}

int GCLScriptLanguage::find_function(const String &p_function, const String &p_code) const {
	return 0;
}

String GCLScriptLanguage::make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const {
	return String();
}

Error GCLScriptLanguage::complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) {
	r_force = false;
	r_call_hint = String();
	return autocomplete_run(p_code, p_owner, r_options);
}

} // namespace gcl
