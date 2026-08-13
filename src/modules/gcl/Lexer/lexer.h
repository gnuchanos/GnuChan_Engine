/**************************************************************************/
/*  lexer.h                                                               */
/**************************************************************************/
/*  GCL Lexer - Moduler token bildirimleri.                               */
/*  SADECE bildirim; implementasyon .cpp tarafindadir.                    */
/**************************************************************************/

#ifndef GCL_LEXER_H
#define GCL_LEXER_H

#include "core/string_builder.h"
#include "core/ustring.h"
#include "core/vector.h"

namespace gcl {

/* Comment + temel token turleri. */
enum class TokenKind : uint8_t {
	COMMENT_LINE,  /* # ... */
	COMMENT_BLOCK, /* #| ... |# */

	KEYWORD,
	IDENTIFIER,
	NUMBER,
	STRING,
	CHAR,
	OPERATOR,
	PUNCT,
	NEWLINE,   /* \n */
	CODE_TEXT, /* yorum disinda kalan icerik */
	EOF_TOKEN,
};

/* Ortak token yapisi. */
struct Token {
	TokenKind kind;
	String text;
	int line;
	int col;

	Token() :
			kind(TokenKind::EOF_TOKEN),
			line(1),
			col(1) {
	}
};

/* Comment modulu: kaynak string'i tarar, comment tokenlarini uretir. */
void comment_tokenize(const String &p_source, Vector<Token> &r_tokens);

/* Comment'lerden arindirilmis kaynak (eski API: gcl_script.cpp kullanir). */
String strip_comments(const String &p_source);

} // namespace gcl

#endif // GCL_LEXER_H
