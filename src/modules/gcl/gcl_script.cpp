/**************************************************************************/
/*  gcl_script.cpp                                                        */
/**************************************************************************/
/*  GCLScript (Script resource), GCLScriptInstance (ScriptInstance) and  */
/*  GCLScriptLanguage (ScriptLanguage) for the GCL language.              */
/*                                                                        */
/*  Engine-facing plumbing ONLY: uses parser (GCLScriptData),             */
/*  interpreter (gcl_interp_call_function) and completion.                */
/**************************************************************************/

#include "gcl_script.h"

#include "core/class_db.h"
#include "core/engine.h"
#include "core/print_string.h"
#include "core/string_name.h"
#include "gcl_completion.h"
#include "gcl_parser.h"
#include "scene/main/node.h"
#include "scene/main/viewport.h"

/* Maps engine callback names to the GCL-side names. GCL scripts use ONLY
 * Ready / Update / UpdatePhysics (no underscore variants). */
static StringName map_callback(const StringName &p_method) {
	if (p_method == "_process") {
		return "Update";
	}
	if (p_method == "_physics_process") {
		return "UpdatePhysics";
	}
	if (p_method == "_ready") {
		return "Ready";
	}
	return p_method;
}

/* ------------------------------------------------------------------ */
/*  GCLScript                                                          */
/* ------------------------------------------------------------------ */

void GCLScript::_bind_methods() {
}

bool GCLScript::can_instance() const {
	return valid;
}

Ref<Script> GCLScript::get_base_script() const {
	return Ref<Script>();
}

bool GCLScript::inherits_script(const Ref<Script> &p_script) const {
	return p_script.ptr() == this;
}

StringName GCLScript::get_instance_base_type() const {
	return "Node";
}

String GCLScript::get_extern_class() const {
	return data.extern_class;
}

ScriptInstance *GCLScript::instance_create(Object *p_this) {
	GCLScriptInstance *instance = memnew(GCLScriptInstance);
	instance->script = Ref<GCLScript>(this);
	instance->set_owner(p_this);
	for (const List<GCLMemberData>::Element *E = data.members.front(); E; E = E->next()) {
		instance->member_vars[E->get().name] = E->get().init;
	}
	p_this->set_script_instance(instance);
	// GCL 101 (gcl_language.md): file-scope statements run once at
	// instantiation time ("print_this_global();").
	if (data.top_level_lines.size() > 0) {
		Map<StringName, Variant> locals;
		String err;
		gcl_exec_body(data.top_level_lines, 0, data.top_level_lines.size(), locals, instance, 1, &err);
		if (err != "") {
			ERR_PRINT("GCL: " + err);
		}
	}
	return instance;
}

bool GCLScript::instance_has(const Object *p_this) const {
	if (!p_this->get_script_instance()) {
		return false;
	}
	return p_this->get_script_instance()->get_script().ptr() == this;
}

bool GCLScript::has_source_code() const {
	return source != "";
}

String GCLScript::get_source_code() const {
	return source;
}

void GCLScript::set_source_code(const String &p_code) {
	source = p_code;
}

Error GCLScript::reload(bool p_keep_state) {
	valid = false;
	String err;
	int err_line = 0;
	Error res = gcl_parse(source, &data, &err, &err_line);
	if (res != OK) {
		ERR_PRINT("GCL: Parse Error: " + err + " (line " + itos(err_line) + ")");
		return res;
	}
	valid = true;
	return OK;
}

bool GCLScript::has_method(const StringName &p_method) const {
	StringName mapped = map_callback(p_method);
	for (const List<GCLFunctionData>::Element *E = data.functions.front(); E; E = E->next()) {
		if (E->get().name == mapped) {
			return true;
		}
	}
	return false;
}

MethodInfo GCLScript::get_method_info(const StringName &p_method) const {
	StringName mapped = map_callback(p_method);
	for (const List<GCLFunctionData>::Element *E = data.functions.front(); E; E = E->next()) {
		if (E->get().name == mapped) {
			MethodInfo mi;
			mi.name = mapped;
			return mi;
		}
	}
	return MethodInfo();
}

bool GCLScript::is_tool() const {
	return false;
}

bool GCLScript::is_valid() const {
	return valid;
}

ScriptLanguage *GCLScript::get_language() const {
	return GCLScriptLanguage::get_singleton();
}

bool GCLScript::has_script_signal(const StringName &p_signal) const {
	return false;
}

void GCLScript::get_script_signal_list(List<MethodInfo> *r_signals) const {
}

bool GCLScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
	return false;
}

void GCLScript::get_script_method_list(List<MethodInfo> *p_list) const {
	for (const List<GCLFunctionData>::Element *E = data.functions.front(); E; E = E->next()) {
		MethodInfo mi;
		mi.name = E->get().name;
		for (int i = 0; i < E->get().arg_names.size(); i++) {
			PropertyInfo ai;
			ai.name = E->get().arg_names[i];
			mi.arguments.push_back(ai);
		}
		p_list->push_back(mi);
	}
}

void GCLScript::get_script_property_list(List<PropertyInfo> *p_list) const {
}

GCLScript::GCLScript() {
	valid = false;
}

/* ------------------------------------------------------------------ */
/*  GCLScriptInstance                                                  */
/* ------------------------------------------------------------------ */

GCLScriptInstance::GCLScriptInstance() {
	owner = nullptr;
	call_depth = 0;
}

bool GCLScriptInstance::enter_call(String *r_error) {
	call_depth++;
	if (call_depth > kMaxCallDepth) {
		if (r_error && *r_error == "") {
			*r_error = "GCL call depth exceeded " + itos(kMaxCallDepth) + " (possible infinite recursion)";
		}
		call_depth--;
		return false;
	}
	return true;
}

void GCLScriptInstance::leave_call() {
	if (call_depth > 0) {
		call_depth--;
	}
}

void GCLScriptInstance::set_owner(Object *p_owner) {
	owner = p_owner;
}

Object *GCLScriptInstance::get_owner() {
	return owner;
}

bool GCLScriptInstance::has_function(const StringName &p_name) const {
	if (script.is_null()) {
		return false;
	}
	for (const List<GCLFunctionData>::Element *E = script->data.functions.front(); E; E = E->next()) {
		if (E->get().name == p_name) {
			return true;
		}
	}
	return false;
}

Variant GCLScriptInstance::call_function(const StringName &p_name, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	r_error.error = Variant::CallError::CALL_OK;
	if (script.is_null()) {
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
		return Variant();
	}
	if (!enter_call(nullptr)) {
		r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
		return Variant();
	}
	Vector<Variant> args;
	for (int i = 0; i < p_argcount; i++) {
		args.push_back(*p_args[i]);
	}
	for (const List<GCLFunctionData>::Element *E = script->data.functions.front(); E; E = E->next()) {
		if (E->get().name == p_name) {
			String err;
			int err_line = 0;
			Variant result = gcl_interp_call_function(this, E->get(), args, &err, &err_line);
			if (err != "") {
				ERR_PRINT("GCL: " + err);
			}
			leave_call();
			return result;
		}
	}
	leave_call();
	r_error.error = Variant::CallError::CALL_ERROR_INVALID_METHOD;
	return Variant();
}

Variant GCLScriptInstance::call_function(const StringName &p_name, const Vector<Variant> &p_args) {
	if (script.is_null()) {
		return Variant();
	}
	for (const List<GCLFunctionData>::Element *E = script->data.functions.front(); E; E = E->next()) {
		if (E->get().name == p_name) {
			String err;
			int err_line = 0;
			Variant result = gcl_interp_call_function(this, E->get(), p_args, &err, &err_line);
			if (err != "") {
				ERR_PRINT("GCL: " + err);
			}
			return result;
		}
	}
	return Variant();
}

bool GCLScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "script") {
		return false;
	}
	member_vars[p_name] = p_value;
	return true;
}

bool GCLScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	const Map<StringName, Variant>::Element *E = member_vars.find(p_name);
	if (E) {
		r_ret = E->get();
		return true;
	}
	return false;
}

void GCLScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
	for (const Map<StringName, Variant>::Element *E = member_vars.front(); E; E = E->next()) {
		PropertyInfo pi;
		pi.name = E->key();
		pi.type = E->get().get_type();
		p_properties->push_back(pi);
	}
}

Variant::Type GCLScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	const Map<StringName, Variant>::Element *E = member_vars.find(p_name);
	if (E) {
		if (r_is_valid) {
			*r_is_valid = true;
		}
		return E->get().get_type();
	}
	if (r_is_valid) {
		*r_is_valid = false;
	}
	return Variant::NIL;
}

void GCLScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
	if (script.is_valid()) {
		script->get_script_method_list(p_list);
	}
}

bool GCLScriptInstance::has_method(const StringName &p_method) const {
	return script.is_valid() && script->has_method(p_method);
}

Variant GCLScriptInstance::call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) {
	if (Engine::get_singleton()->is_editor_hint() && (p_method == "_process" || p_method == "_physics_process" || p_method == "_input" || p_method == "_unhandled_input")) {
		r_error.error = Variant::CallError::CALL_OK;
		return Variant();
	}
	return call_function(map_callback(p_method), p_args, p_argcount, r_error);
}

void GCLScriptInstance::notification(int p_notification) {
	if (script.is_null() || !owner) {
		return;
	}
	switch (p_notification) {
		case Node::NOTIFICATION_ENTER_TREE: {
			if (Engine::get_singleton()->is_editor_hint()) {
				owner->call("set_process", false);
				owner->call("set_physics_process", false);
			}
		} break;
		case Node::NOTIFICATION_READY: {
			if (Engine::get_singleton()->is_editor_hint()) {
				owner->call("set_process", false);
				owner->call("set_physics_process", false);
				break;
			}
			if (script->has_method("_process")) {
				owner->call("set_process", true);
			}
			if (script->has_method("_physics_process")) {
				owner->call("set_physics_process", true);
			}
		} break;
		default:
			break;
	}
}

Ref<Script> GCLScriptInstance::get_script() const {
	return script;
}

ScriptLanguage *GCLScriptInstance::get_language() {
	return GCLScriptLanguage::get_singleton();
}

MultiplayerAPI::RPCMode GCLScriptInstance::get_rpc_mode(const StringName &p_method) const {
	return MultiplayerAPI::RPC_MODE_DISABLED;
}

MultiplayerAPI::RPCMode GCLScriptInstance::get_rset_mode(const StringName &p_variable) const {
	return MultiplayerAPI::RPC_MODE_DISABLED;
}

/* ------------------------------------------------------------------ */
/*  GCLScriptLanguage                                                  */
/* ------------------------------------------------------------------ */

GCLScriptLanguage *GCLScriptLanguage::singleton = nullptr;

GCLScriptLanguage *GCLScriptLanguage::get_singleton() {
	return singleton;
}

String GCLScriptLanguage::get_name() const {
	return "GCL";
}

void GCLScriptLanguage::init() {
}

String GCLScriptLanguage::get_type() const {
	return "GCLScript";
}

String GCLScriptLanguage::get_extension() const {
	return "gcl";
}

Error GCLScriptLanguage::execute_file(const String &p_path) {
	return OK;
}

void GCLScriptLanguage::finish() {
}

void GCLScriptLanguage::get_reserved_words(List<String> *p_words) const {
	static const char *reserved[] = {
		"void", "if", "else", "elif", "while", "for", "return",
		"true", "false", "printf", "null", "nullptr", "NODE",
		"self", "Engine", "Input", "var", "func", "class", "extern",
		nullptr
	};
	const char **w = reserved;
	while (*w) {
		p_words->push_back(*w);
		w++;
	}
}

bool GCLScriptLanguage::is_control_flow_keyword(String p_keyword) const {
	return p_keyword == "if" || p_keyword == "else" || p_keyword == "elif" || p_keyword == "while" || p_keyword == "for" || p_keyword == "return";
}

void GCLScriptLanguage::get_comment_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("#");
	p_delimiters->push_back("#| |#");
}

void GCLScriptLanguage::get_string_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("\" \"");
}

Ref<Script> GCLScriptLanguage::get_template(const String &p_class_name, const String &p_base_class_name) const {
	String template_source;
	if (p_class_name != "") {
		template_source += "@extern " + p_class_name + "\n\n";
	}
	template_source += "void Ready() {\n\n}\n\nvoid Update(float delta) {\n\n}\n\nvoid UpdatePhysics(float delta) {\n\n}\n";

	Ref<GCLScript> script = memnew(GCLScript);
	script->set_source_code(template_source);
	script->reload();
	return script;
}

bool GCLScriptLanguage::validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path, List<String> *r_functions, List<Warning> *r_warnings, Set<int> *r_safe_lines) const {
	GCLScriptData data;
	String err;
	int err_line = 0;
	Error res = gcl_parse(p_script, &data, &err, &err_line);
	if (res != OK) {
		r_line_error = err_line;
		r_col_error = 0;
		r_test_error = err;
		return false;
	}
	if (r_functions) {
		for (const List<GCLFunctionData>::Element *E = data.functions.front(); E; E = E->next()) {
			r_functions->push_back(E->get().name);
		}
	}
	return true;
}

Script *GCLScriptLanguage::create_script() const {
	return memnew(GCLScript);
}

bool GCLScriptLanguage::has_named_classes() const {
	return false;
}

bool GCLScriptLanguage::supports_builtin_mode() const {
	return true;
}

int GCLScriptLanguage::find_function(const String &p_function, const String &p_code) const {
	GCLScriptData data;
	String err;
	int err_line = 0;
	if (gcl_parse(p_code, &data, &err, &err_line) != OK) {
		return -1;
	}
	for (const List<GCLFunctionData>::Element *E = data.functions.front(); E; E = E->next()) {
		if (E->get().name == p_function) {
			return E->get().line;
		}
	}
	return -1;
}

String GCLScriptLanguage::make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const {
	String s = "void " + p_name + "(";
	for (int i = 0; i < p_args.size(); i++) {
		if (i > 0) {
			s += ", ";
		}
		s += p_args[i];
	}
	s += ") {\n\n}\n";
	return s;
}

void GCLScriptLanguage::auto_indent_code(String &p_code, int p_from_line, int p_to_line) const {
	Vector<String> lines = p_code.split("\n");
	bool prev_brace = false;
	for (int i = 0; i < lines.size(); i++) {
		if (prev_brace && i >= p_from_line && i <= p_to_line) {
			lines.write[i] = "\t" + lines[i];
		}
		prev_brace = lines[i].strip_edges().ends_with("{");
	}
	p_code = "";
	for (int i = 0; i < lines.size(); i++) {
		if (i > 0) {
			p_code += "\n";
		}
		p_code += lines[i];
	}
}

Error GCLScriptLanguage::complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) {
	return gcl_complete_code(p_code, p_path, p_owner, r_options, r_force, r_call_hint);
}

void GCLScriptLanguage::add_global_constant(const StringName &p_variable, const Variant &p_value) {
}

String GCLScriptLanguage::debug_get_error() const {
	return String();
}

int GCLScriptLanguage::debug_get_stack_level_count() const {
	return 0;
}

int GCLScriptLanguage::debug_get_stack_level_line(int p_level) const {
	return 0;
}

String GCLScriptLanguage::debug_get_stack_level_function(int p_level) const {
	return String();
}

String GCLScriptLanguage::debug_get_stack_level_source(int p_level) const {
	return String();
}

void GCLScriptLanguage::debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {
}

void GCLScriptLanguage::debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {
}

void GCLScriptLanguage::debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems, int p_max_depth) {
}

String GCLScriptLanguage::debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems, int p_max_depth) {
	return String();
}

void GCLScriptLanguage::reload_all_scripts() {
}

void GCLScriptLanguage::reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) {
}

void GCLScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("gcl");
}

void GCLScriptLanguage::get_public_functions(List<MethodInfo> *p_functions) const {
	MethodInfo printf_mi;
	printf_mi.name = "printf";
	PropertyInfo arg;
	arg.name = "text";
	arg.type = Variant::STRING;
	printf_mi.arguments.push_back(arg);
	p_functions->push_back(printf_mi);
}

void GCLScriptLanguage::get_public_constants(List<Pair<String, Variant>> *p_constants) const {
}

void GCLScriptLanguage::profiling_start() {
}

void GCLScriptLanguage::profiling_stop() {
}

int GCLScriptLanguage::profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) {
	return 0;
}

int GCLScriptLanguage::profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) {
	return 0;
}

GCLScriptLanguage::GCLScriptLanguage() {
	ERR_FAIL_COND(singleton);
	singleton = this;
}

GCLScriptLanguage::~GCLScriptLanguage() {
	singleton = nullptr;
}
