/**************************************************************************/
/*  gcl_lexer.h                                                           */
/**************************************************************************/
/*  Tokenizer for the GCL language: converts source text into tokens.     */
/*                                                                        */
/*  This module does NOT know about the engine or the AST: it only        */
/*  splits text into GCLToken items.                                      */
/**************************************************************************/

#ifndef GCL_LEXER_H
#define GCL_LEXER_H

#include "core/error_list.h"
#include "gcl_core.h"

/* Tokenizes p_source. On success returns OK and fills r_tokens.        */
/* On failure returns ERR_PARSE_ERROR and fills r_error / r_err_line.   */
Error gcl_lex(const String &p_source, List<GCLToken> *r_tokens, String *r_error = nullptr, int *r_err_line = nullptr);

#endif // GCL_LEXER_H
