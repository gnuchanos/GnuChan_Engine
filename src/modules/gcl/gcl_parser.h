/**************************************************************************/
/*  gcl_parser.h                                                          */
/**************************************************************************/
/*  Parser: GCLToken stream -> GCLScriptData (functions + members +      */
/*  extern class name).                                                   */
/*                                                                        */
/*  This module works on tokens only; it does not touch the engine.       */
/**************************************************************************/

#ifndef GCL_PARSER_H
#define GCL_PARSER_H

#include "core/error_list.h"
#include "gcl_core.h"

/* Parses GCL source (tokenizing internally) into p_data.
 * Returns OK or ERR_PARSE_ERROR with error message/line. */
Error gcl_parse(const String &p_source, GCLScriptData *r_data, String *r_error = nullptr, int *r_err_line = nullptr);

/* Convenience: parse raw member/function info for editors (duplicates
 * GCLScriptData but delivered as plain lists for ScriptLanguage APIs). */
void gcl_parse_functions(const String &p_source, List<GCLFunctionData> *r_functions, String *r_extern_class = nullptr, List<GCLMemberData> *r_members = nullptr);

#endif // GCL_PARSER_H
