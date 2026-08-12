/**************************************************************************/
/*  gcl_core.h                                                            */
/**************************************************************************/
/*  Shared types for the GCL language modules.                            */
/*                                                                        */
/*  This header is the single source of truth for the AST data            */
/*  structures. Each module includes it and works on these types:         */
/*    gcl_lexer       -> produces tokens                                    */
/*    gcl_parser      -> consumes tokens, produces GCLScriptData           */
/*    gcl_interpreter -> executes GCLScriptData                            */
/*    gcl_script      -> ScriptLanguage plumbing (Script/Instance)         */
/*    gcl_completion  -> editor autocomplete over GCLScriptData            */
/**************************************************************************/

#ifndef GCL_CORE_H
#define GCL_CORE_H

#include "core/array.h"
#include "core/dictionary.h"
#include "core/list.h"
#include "core/map.h"
#include "core/string_name.h"
#include "core/ustring.h"
#include "core/variant.h"
#include "core/vector.h"

class Node;

/* ------------------------------------------------------------------ */
/*  Tokens (gcl_lexer)                                                 */
/* ------------------------------------------------------------------ */

enum GCLTokenType {
	GCL_TOKEN_EOF,
	GCL_TOKEN_IDENTIFIER,
	GCL_TOKEN_NUMBER,
	GCL_TOKEN_STRING,
	GCL_TOKEN_CHAR,
	GCL_TOKEN_PUNCTUATOR, // ( ) { } [ ] , ; .
	GCL_TOKEN_OPERATOR, // = + - * / % == != < > <= >= && || ! & | ^ ~ << >> ++ -- += -= *= /= %= -> .  (dot handled separately)
	GCL_TOKEN_DIRECTIVE, // #include #lib #extern #define #ifdef ...
	GCL_TOKEN_EXTERN_CLASS, // @extern ClassName
};

struct GCLToken {
	GCLTokenType type;
	String text; // raw lexeme
	int line;

	GCLToken() {
		type = GCL_TOKEN_EOF;
		line = 0;
	}
};

/* ------------------------------------------------------------------ */
/*  AST (gcl_parser)                                                   */
/* ------------------------------------------------------------------ */

/* A parsed GCL function body. The interpreter executes it line by line. */
struct GCLFunctionData {
	StringName name;
	Vector<String> arg_names;
	Vector<String> body_lines; // raw source lines, preprocessed per statement
	int line;

	GCLFunctionData() {
		line = 0;
	}
};

/* A top-level member declaration with its initial value:
 *   bool HandFull = false;   (bool)
 *   NODE OBJ = NULL;         (nil node ref)
 * Stores the name and a literal initial value. */
struct GCLMemberData {
	StringName name;
	Variant init;

	GCLMemberData() {}
};

/* External binding registered through gcl_language.md directives:
 *   #extern "raylib.dll"           -> library_native (dll/so/a path)
 *   #register void InitWindow(...) -> signature (full declaration text)
 * The binding is kept so scripts can resolve names like InitWindow. */
struct GCLExternBinding {
	String library_native;
	String signature;

	GCLExternBinding() {}
};

/* struct Player { char name[32]; int health; float speed; }  (gcl_language.md) */
struct GCLStructData {
	String name;
	Vector<String> field_names;
	Vector<String> field_types;

	GCLStructData() {}
};

/* enum Color { RED, GREEN, BLUE }  (gcl_language.md) */
struct GCLEnumData {
	String name;
	Vector<String> values;

	GCLEnumData() {}
};

/* typedef int32_t int32;  (gcl_language.md) */
struct GCLTypedefData {
	String new_name;
	String target;

	GCLTypedefData() {}
};

/* Complete parse result for one .gcl file. */
struct GCLScriptData {
	String extern_class; // from "@extern ClassName", may be empty
	List<GCLFunctionData> functions;
	List<GCLMemberData> members;
	List<GCLExternBinding> extern_bindings; // from #extern / #register
	Vector<String> top_level_lines; // file-scope statements (GCL 101: print_this_global();)
	List<GCLStructData> structs; // struct declarations
	List<GCLEnumData> enums; // enum declarations
	List<GCLTypedefData> typedefs; // typedef declarations
};

class GCLScriptInstance;

/* --- runtime support (gcl_input.cpp) --- */
bool gcl_is_down(const String &p_arg);
bool gcl_is_up(const String &p_arg);
int gcl_keycode_from_name(const String &p_name);

/* --- runtime support (gcl_printf.cpp) --- */
String gcl_printf_format(const String &p_fmt, const Vector<Variant> &p_args);
void gcl_printf_output(const String &p_text);

/* --- runtime support (gcl_nodes.cpp) --- */
Node *gcl_find_node_ci(Node *p_from, const String &p_name);
Variant gcl_find_node(GCLScriptInstance *p_inst, const String &p_name, String *r_error, int p_line);
Variant gcl_resolve_path(GCLScriptInstance *p_inst, const String &p_path, String *r_error, int p_line);
Variant gcl_variant_resolve_node(GCLScriptInstance *p_inst, const Variant &p_v);

/* --- expression evaluation (gcl_expr.cpp) --- */
Variant gcl_eval_expr(const String &p_expr, const Map<StringName, Variant> &p_locals, GCLScriptInstance *p_inst, int p_line, String *r_error);
void gcl_write_member(GCLScriptInstance *p_inst, const String &p_target, const Variant &p_value, const Map<StringName, Variant> &p_locals, int p_line, String *r_error);
Vector<String> gcl_split_top_commas(const String &p_s);
Vector<String> gcl_split_member_path(const String &p_s);
String gcl_strip_inline_comment(const String &p_line);

/* --- statement execution (gcl_stmt.cpp) --- */
struct GCLExecResult {
	bool ret;
	Variant ret_value;
	bool break_loop;
	bool continue_loop;

	GCLExecResult() {
		ret = false;
		break_loop = false;
		continue_loop = false;
	}
};
GCLExecResult gcl_exec_body(const Vector<String> &p_lines, int p_start, int p_end, Map<StringName, Variant> &p_locals, GCLScriptInstance *p_inst, int p_base_line, String *r_error);

#endif // GCL_CORE_H
