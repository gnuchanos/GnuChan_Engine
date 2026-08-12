/**************************************************************************/
/*  gcl_interpreter.h                                                     */
/**************************************************************************/
/*  Interpreter for the GCL language: executes parsed functions.          */
/*                                                                        */
/*  Works on GCLScriptData only; the engine-facing GCLScriptInstance      */
/*  lives in gcl_script.cpp and calls into this module.                   */
/**************************************************************************/

#ifndef GCL_INTERPRETER_H
#define GCL_INTERPRETER_H

#include "core/map.h"
#include "core/ustring.h"
#include "core/variant.h"
#include "core/vector.h"
#include "gcl_core.h"

class GCLScriptInstance;

/* Executes a parsed GCL function body on the given instance.
 * Returns the function's return value (or NIL). */
Variant gcl_interp_call_function(GCLScriptInstance *p_instance, const GCLFunctionData &p_func, const Vector<Variant> &p_args, String *r_error = nullptr, int *r_error_line = nullptr);

#endif // GCL_INTERPRETER_H
