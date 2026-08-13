/**************************************************************************/
/*  gcl_script.h                                                          */
/**************************************************************************/
/*  GCL registered as a Godot ScriptLanguage.                             */
/*  Extensions (gcl_language.md "files"):                                 */
/*    .gcsf   normal GCL script file                                      */
/*    .gclib  modular GCL library file                                    */
/*  Ayrica: otomatik tamamlama (complete_code) ve self sistemi            */
/*    @extern <node>  -> self.<node> erisimi                              */
/*    Ready / Update / PhysicsUpdate  -> isletim fonksiyonlari            */
/**************************************************************************/

#ifndef GCL_SCRIPT_H
#define GCL_SCRIPT_H

#include "core/io/multiplayer_api.h"
#include "core/map.h"
#include "core/object.h"
#include "core/pool_vector.h"
#include "core/script_language.h"
#include "core/set.h"

#include "Executor/executor.h"

namespace gcl {

/* Autocomplete modulu: GCL kodu icin oneriler uretir.
   p_owner: script'in bagli oldugu nesne (sahnedeki node adlari icin). */
Error autocomplete_run(const String &p_code, Object *p_owner, List<ScriptCodeCompletionOption> *r_options);

class GCLScriptInstance;

class GCLScript : public Script {
	GDCLASS(GCLScript, Script);

	String source_code;
	Set<Object *> instances;

protected:
	static void _bind_methods();

public:
	bool can_instance() const override { return true; }
	Ref<Script> get_base_script() const override { return Ref<Script>(); }
	bool inherits_script(const Ref<Script> &p_script) const override { return false; }
	StringName get_instance_base_type() const override { return StringName("Node"); }
	ScriptInstance *instance_create(Object *p_this) override;
	bool instance_has(const Object *p_this) const override { return instances.has((Object *)p_this); }

	bool has_source_code() const override { return !source_code.empty(); }
	String get_source_code() const override { return source_code; }
	void set_source_code(const String &p_code) override { source_code = p_code; }
	Error reload(bool p_keep_state = false) override { return OK; }

	bool has_method(const StringName &p_method) const override { return false; }
	MethodInfo get_method_info(const StringName &p_method) const override { return MethodInfo(); }

	bool is_tool() const override { return false; }
	bool is_valid() const override { return true; }

	ScriptLanguage *get_language() const override;

	bool has_script_signal(const StringName &p_signal) const override { return false; }
	void get_script_signal_list(List<MethodInfo> *r_signals) const override {}
	bool get_property_default_value(const StringName &p_property, Variant &r_value) const override { return false; }
	void get_script_method_list(List<MethodInfo> *p_list) const override {}
	void get_script_property_list(List<PropertyInfo> *p_list) const override {}
};

class GCLScriptInstance : public ScriptInstance {
	Object *owner;
	Ref<GCLScript> script;

	/* self sistemi: property adi -> deger (degiskenler + extern node'lar). */
	Map<StringName, Variant> members;

	/* typed tanimlari: typedef/enum/struct/union kayitlari. */
	GCLTypeRegistry types;

public:
	GCLScriptInstance(Object *p_owner, Ref<GCLScript> p_script);
	~GCLScriptInstance() {}

	bool set(const StringName &p_name, const Variant &p_value) override;
	bool get(const StringName &p_name, Variant &r_ret) const override;
	void get_property_list(List<PropertyInfo> *p_properties) const override;
	Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = nullptr) const override;

	void get_method_list(List<MethodInfo> *p_list) const override;
	bool has_method(const StringName &p_method) const override;
	Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error) override;
	void notification(int p_notification) override;

	Ref<Script> get_script() const override { return script; }
	MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const override { return MultiplayerAPI::RPC_MODE_DISABLED; }
	MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const override { return MultiplayerAPI::RPC_MODE_DISABLED; }
	ScriptLanguage *get_language() override { return script->get_language(); }

	/* self sistemi: script kaynagindaki @extern ve degiskenleri yukler. */
	void load_members();
};

class GCLScriptLanguage : public ScriptLanguage {
	static GCLScriptLanguage *singleton;

public:
	static GCLScriptLanguage *get_singleton() { return singleton; }
	GCLScriptLanguage() { singleton = this; }
	~GCLScriptLanguage() {
		if (singleton == this) {
			singleton = nullptr;
		}
	}

	String get_name() const override { return "GCL"; }
	void init() override {}
	String get_type() const override { return "GCL"; }
	String get_extension() const override { return "gcsf"; }
	Error execute_file(const String &p_path) override;
	void finish() override {}

	void get_reserved_words(List<String> *p_words) const override;
	bool is_control_flow_keyword(String p_string) const override;
	void get_comment_delimiters(List<String> *p_delimiters) const override {
		p_delimiters->push_back("#");
		p_delimiters->push_back("#|");
		p_delimiters->push_back("|#");
	}
	void get_string_delimiters(List<String> *p_delimiters) const override {
		p_delimiters->push_back("\"");
	}
	Ref<Script> get_template(const String &p_class_name, const String &p_base_class_name) const override;
	bool validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path = "", List<String> *r_functions = nullptr, List<Warning> *r_warnings = nullptr, Set<int> *r_safe_lines = nullptr) const override;
	Script *create_script() const override;
	bool has_named_classes() const override { return false; }
	bool supports_builtin_mode() const override { return false; }
	int find_function(const String &p_function, const String &p_code) const override;
	String make_function(const String &p_class, const String &p_name, const PoolStringArray &p_args) const override;
	Error complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) override;
	void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const override {}

	void add_global_constant(const StringName &p_variable, const Variant &p_value) override {}

	String debug_get_error() const override { return String(); }
	int debug_get_stack_level_count() const override { return 0; }
	int debug_get_stack_level_line(int p_level) const override { return 0; }
	String debug_get_stack_level_function(int p_level) const override { return String(); }
	String debug_get_stack_level_source(int p_level) const override { return String(); }
	void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override {}
	void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override {}
	void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override {}
	String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1) override { return p_expression; }

	void reload_all_scripts() override {}
	void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) override {}

	void get_recognized_extensions(List<String> *p_extensions) const override {
		p_extensions->push_back("gcsf");
		p_extensions->push_back("gclib");
	}
	void get_public_functions(List<MethodInfo> *p_functions) const override {}
	void get_public_constants(List<Pair<String, Variant>> *p_constants) const override {}

	void profiling_start() override {}
	void profiling_stop() override {}
	int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) override { return 0; }
	int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) override { return 0; }
};

} // namespace gcl

#endif // GCL_SCRIPT_H
