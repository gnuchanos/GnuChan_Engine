/**************************************************************************/
/*  gcl_preprocessor.h                                                     */
/**************************************************************************/
/*  GCL preprocessor (gcl_language.md "Dil Sozdizimi" bolumu).            */
/*                                                                        */
/*  Handles before parsing:                                               */
/*    #include "file.gcsf" | <file.gcsf>  -> inline file (res:// aware)  */
/*    #lib "file.gclib"    | <file.gclib>  -> inline library file         */
/*    #define NAME value  / #undef NAME   -> object-like macros           */
/*    #ifdef / #ifndef / #if / #elif / #else / #endif  -> compile switch  */
/*    #warning "text"   -> yellow console message                         */
/*    #error   "text"   -> red console message + parse failure            */
/*    #debug   "text"   -> blue console message                           */
/*    #register / #extern -> external binding declarations (stripped)     */
/*                                                                        */
/*  Ordinary '#' comment lines are left untouched for the lexer.          */
/**************************************************************************/

#ifndef GCL_PREPROCESSOR_H
#define GCL_PREPROCESSOR_H

#include "core/error_list.h"
#include "core/ustring.h"

/* Runs the preprocessor over p_source. On success returns OK and fills
 * r_out with the processed source. On failure (#error, bad conditional,
 * missing include) returns ERR_PARSE_ERROR and fills r_error / r_err_line. */
Error gcl_preprocess(const String &p_source, String *r_out, String *r_error = nullptr, int *r_err_line = nullptr);

#endif // GCL_PREPROCESSOR_H
