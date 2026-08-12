/**************************************************************************/
/*  gcl_completion.h                                                      */
/**************************************************************************/
/*  Code completion for the GCL scripting language.                       */
/*                                                                        */
/*  This translation unit is the SINGLE place that builds code            */
/*  completion options. GCLScriptLanguage::complete_code() simply         */
/*  delegates here.                                                       */
/**************************************************************************/

#ifndef GCL_COMPLETION_H
#define GCL_COMPLETION_H

#include "core/error_list.h"
#include "core/script_language.h"
#include "core/ustring.h"

/* Builds autocomplete options for the given GCL source snippet. */
Error gcl_complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_force, String &r_call_hint);

#endif // GCL_COMPLETION_H
