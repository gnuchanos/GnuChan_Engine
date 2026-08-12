/**************************************************************************/
/*  gcl_interpreter.cpp                                                   */
/**************************************************************************/
/*  Interpreter entry point. The heavy lifting lives in small modules:    */
/*    gcl_stmt.cpp   -> statement execution (if/while/for/assign)         */
/*    gcl_expr.cpp   -> expression evaluation (member paths, literals)    */
/*    gcl_nodes.cpp  -> node lookup ($path, FindNode, NodePath)           */
/*    gcl_input.cpp  -> Input.IsDown / IsKeyDown                          */
/*    gcl_printf.cpp -> printf formatting                                 */
/**************************************************************************/

#include "gcl_interpreter.h"

#include "gcl_script.h"

Variant gcl_interp_call_function(GCLScriptInstance *p_instance, const GCLFunctionData &p_func, const Vector<Variant> &p_args, String *r_error, int *r_error_line) {
	if (!p_instance) {
		return Variant();
	}
	Map<StringName, Variant> locals;
	for (int i = 0; i < p_func.arg_names.size() && i < p_args.size(); i++) {
		locals[StringName(p_func.arg_names[i])] = p_args[i];
	}
	GCLExecResult res = gcl_exec_body(p_func.body_lines, 0, p_func.body_lines.size(), locals, p_instance, p_func.line, r_error);
	if (r_error && *r_error != "" && r_error_line) {
		*r_error_line = p_func.line;
	}
	return res.ret_value;
}
