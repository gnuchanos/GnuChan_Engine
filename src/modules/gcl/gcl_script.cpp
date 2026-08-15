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
#include "core/error_macros.h"
#include "core/object.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "core/print_string.h"
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
	/* BOS: "Script Variables" bolumunu Inspector'da HIC gosterme.
	   GCL degiskenleri (/extern sinif adlari, self, delta dahil) yorumlayicinin
	   ic members tablosudur; Inspector'da sergilenmeleri "[null]" gibi
	   anlamsiz satirlar uretiyordu (Node References surukle-birak ile
	   karsisiyordu). Kullanici node baglantisini zaten editor uzerinden yapar. */
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
		/* Gercek kare suresini (delta) gec: 'Body.Rotation.y += 100 * delta'
		   icinde delta 0 kalmasin. */
		Variant::CallError err;
		Variant delta = 1.0 / 60.0;
		Node *node = Object::cast_to<Node>(owner);
		if (node) {
			delta = node->get_process_delta_time();
		}
		const Variant *args[1] = { &delta };
		call("Update", args, 1, err);
	} else if (p_notification == Node::NOTIFICATION_PHYSICS_PROCESS) {
		Variant::CallError err;
		Variant delta = 1.0 / 60.0;
		Node *node = Object::cast_to<Node>(owner);
		if (node) {
			delta = node->get_physics_process_delta_time();
		}
		const Variant *args[1] = { &delta };
		call("UpdatePhysics", args, 1, err);
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
		/* self sistemi: script'in bagli oldugu Godot nesnesini members'a yaz.
		   self.Raycast.IsColliding gibi zincirler executor_core::evaluate_expr
		   icinde OBJECT uzerinden cozulur. */
		members[StringName("self")] = owner ? Variant((Object *)owner) : Variant();

		/* Parametre baglama: Update/UpdatePhysics birinci argumani (delta)
		   members'a yaz ki 'Body.Rotation.y += 100 * delta' gercek delta
		   ile toplasin. Kucuk harf VARSAYIM: GCL imzasi 'float delta' */
		if (p_method == "Update" || p_method == "UpdatePhysics") {
			if (p_args && p_argcount >= 1 && p_args[0]) {
				members[StringName("delta")] = *p_args[0];
			}
		}

		if (p_method == "Ready") {
			String global_code = executor_strip_bodies(code);
			executor_run(global_code, members, types);
		}

		/* Time.Sleep non-blocking devam. Sleep YALNIZCA icinde oldugu
		   FONKSIYONUN kalan satirlarini erteletir; Update/UpdatePhysics
		   govdesi beklerken HER FRAME CALISMAYA DEVAM EDER (raycast/
		   donme/spawn gibi isler donmaz). Sure dolunca sleep_rest
		   kopyalanan scope ile bir kez calistirilir. */
		if (types.sleep_pending && !types.sleep_rest.empty()) {
			double now = (double)OS::get_singleton()->get_ticks_msec();
			if (now >= types.sleep_until) {
				/* Kalan satirlari saklanmis scope'la calistir. */
				Map<StringName, Variant> resume_scope;
				for (const Map<StringName, Variant>::Element *E = types.sleep_scope.front(); E; E = E->next()) {
					resume_scope[E->key()] = E->get();
				}
				String rest = types.sleep_rest;
				types.sleep_rest = String();
				types.sleep_pending = false;
				types.sleep_armed = false;
				types.sleep_rest_captured = false;
				executor_run(rest, resume_scope, types);
				/* yerel degisken/parametre degisikliklerini members'a geri yaz */
				for (const Map<StringName, Variant>::Element *E = resume_scope.front(); E; E = E->next()) {
					members[E->key()] = E->get();
				}
			}
		} else if (types.sleep_pending) {
			/* korunma: rest bos ise bekleme durumunu temizle. */
			types.sleep_pending = false;
			types.sleep_armed = false;
			types.sleep_rest_captured = false;
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
	/* CLI'dan .gcsf calistirma: dosyayi oku, global scope'u (+ Ready varsa
	   govdesini) yorumla. self=null; Time.Sleep CLI'da tek seferde degerlendirilir. */
	FileAccess *file = FileAccess::open(p_path, FileAccess::READ);
	if (!file) {
		ERR_PRINT("GCL: cannot open '" + p_path + "'.");
		return ERR_CANT_OPEN;
	}
	String code = file->get_as_utf8_string();
	file->close();
	memdelete(file);

	/* Yorumlar + preprocessor satirlari icin yorum temizligi: strip_comments
	   #define gibi direktifleri de siler, bu yuzden global scope'da yeniden
	   islenecek govdelerde preprocessor yok sayilir (CLI icin kabul edilebilir). */
	String clean = gcl::strip_comments(code);

	GCLTypeRegistry types;
	types.source = clean;
	types.call_depth = 0;

	Map<StringName, Variant> members;
	members[StringName("self")] = Variant();

	/* Global scope atamalari ve kontrol akisi calistir. */
	String global_code = executor_strip_bodies(clean);
	executor_run(global_code, members, types);

	/* Ready govdesi varsa calistir (CLI'da Update/UpdatePhysics frame'e bagli,
	   Ready yoksa da global scope ile sinirli kalir). */
	String body;
	if (executor_find_body(clean, "Ready", body)) {
		executor_run(body, members, types);
	}

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

/* Basit denge denetimi: { } ( ) [ ] ve string kapanisi. */
namespace {

struct GCLValidateResult {
	bool ok = true;
	int line = 0;
	int col = 0;
	String msg;
};

/* Yorumları ve string'leri hesaba katarak parantez/braket denge ve
   string kapanis denetimi yapar. Fonksiyon govdeleri tam olarak
   ayristirilmaz; yalnizca yapisal denge. */
void gcl_validate_struct(const String &p_code, GCLValidateResult &r_out) {
	int line = 1;
	int col = 1;
	int paren = 0;
	int brace = 0;
	int bracket = 0;
	bool in_str = false;
	const int L = p_code.length();
	int i = 0;
	while (i < L) {
		CharType c = p_code[i];
		if (c == '\n') {
			line++;
			col = 1;
			i++;
			continue;
		}
		if (in_str) {
			if (c == '\\') {
				i += 2;
				col += 2;
				continue;
			}
			if (c == '"') {
				in_str = false;
			}
			i++;
			col++;
			continue;
		}
		if (c == '#' && i + 1 < L && p_code[i + 1] == '|') {
			/* blok yorum: #| ... |# (ic ice) */
			int depth = 1;
			i += 2;
			col += 2;
			while (i < L && depth > 0) {
				if (p_code[i] == '#' && i + 1 < L && p_code[i + 1] == '|') {
					depth++;
					i += 2;
					col += 2;
				} else if (p_code[i] == '|' && i + 1 < L && p_code[i + 1] == '#') {
					depth--;
					i += 2;
					col += 2;
				} else if (p_code[i] == '\n') {
					line++;
					col = 1;
					i++;
				} else {
					i++;
					col++;
				}
			}
			if (depth > 0) {
				r_out.ok = false;
				r_out.line = line;
				r_out.col = col;
				r_out.msg = "Unterminated block comment (missing |#).";
				return;
			}
			continue;
		}
		if (c == '#') {
			/* satir yorumu */
			while (i < L && p_code[i] != '\n') {
				i++;
				col++;
			}
			continue;
		}
		if (c == '"') {
			in_str = true;
			i++;
			col++;
			continue;
		}
		if (c == '(') {
			paren++;
		} else if (c == ')') {
			paren--;
			if (paren < 0) {
				r_out.ok = false;
				r_out.line = line;
				r_out.col = col;
				r_out.msg = "Unbalanced ')'.";
				return;
			}
		} else if (c == '{') {
			brace++;
		} else if (c == '}') {
			brace--;
			if (brace < 0) {
				r_out.ok = false;
				r_out.line = line;
				r_out.col = col;
				r_out.msg = "Unbalanced '}'.";
				return;
			}
		} else if (c == '[') {
			bracket++;
		} else if (c == ']') {
			bracket--;
			if (bracket < 0) {
				r_out.ok = false;
				r_out.line = line;
				r_out.col = col;
				r_out.msg = "Unbalanced ']'.";
				return;
			}
		}
		i++;
		col++;
	}
	if (in_str) {
		r_out.ok = false;
		r_out.line = line;
		r_out.col = col;
		r_out.msg = "Unterminated string literal.";
		return;
	}
	if (brace != 0) {
		r_out.ok = false;
		r_out.line = line;
		r_out.col = col;
		r_out.msg = brace > 0 ? "Unclosed '{' block." : "Unbalanced '}'.";
		return;
	}
	if (paren != 0) {
		r_out.ok = false;
		r_out.line = line;
		r_out.col = col;
		r_out.msg = paren > 0 ? "Unclosed '('." : "Unbalanced ')'.";
		return;
	}
	if (bracket != 0) {
		r_out.ok = false;
		r_out.line = line;
		r_out.col = col;
		r_out.msg = bracket > 0 ? "Unclosed '['." : "Unbalanced ']'.";
	}
}

} // namespace

bool GCLScriptLanguage::validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path, List<String> *r_functions, List<Warning> *r_warnings, Set<int> *r_safe_lines) const {
	/* Yapısal denge: parantez/braket/string kapanislari. */
	GCLValidateResult res;
	gcl_validate_struct(p_script, res);
	if (!res.ok) {
		r_line_error = res.line;
		r_col_error = res.col;
		r_test_error = res.msg;
		return false;
	}

	/* Yorumlari ayiklayip kalinti birakmiyor mu bak; hata mesaji uretmez,
	   yalnizca editor gecisleri icin strip edilmis hali onbellememize gerek yok. */
	gcl::strip_comments(p_script);

	/* Bilinen fonksiyonlarin imzalarini dogrula: Ready() / Update(delta) /
	   UpdatePhysics(delta) -- isim bulunursa govde kapali olmali (zaten brace
	   dengesi bunu garantiler). */

	/* Fonksiyon listesini doldur (editor aramalari icin). */
	if (r_functions) {
		Vector<String> names;
		names.push_back("Ready");
		names.push_back("Update");
		names.push_back("UpdatePhysics");
		for (int i = 0; i < names.size(); i++) {
			if (p_script.find(names[i]) != -1) {
				r_functions->push_back(names[i]);
			}
		}
	}

	/* Bu noktaya gelindiyse dort okunur; yorumlayici calisma aninda tek
	   satirlik dirayetli olmayan cevrimleri zaten sessizce gecirir. */
	r_line_error = 0;
	r_col_error = 0;
	return true;
}

int GCLScriptLanguage::find_function(const String &p_function, const String &p_code) const {
	return 0;
}

String GCLScriptLanguage::make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const {
	return String();
}

Error GCLScriptLanguage::complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) {
	/* r_force = true: Ctrl+Space / tamamlama tetigi her durumda pencereyi
	   acsin. false iken editor bazi durumlarda listeyi hic gostermiyordu. */
	r_force = true;
	r_call_hint = String();

	return autocomplete_run(p_code, p_owner, r_options);
}

} // namespace gcl
