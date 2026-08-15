/**************************************************************************/
/*  comment.cpp                                                           */
/**************************************************************************/
/*  GCL Lexer - Comment modulu (gcl_language.md "comments").              */
/*    #            -> COMMENT_LINE tokeni                                  */
/*    #| ... |#    -> COMMENT_BLOCK tokeni (ic ice acilabilir)            */
/*  Kaynak string'i tarar, comment tokenlarini uretir.                    */
/**************************************************************************/

#include "lexer.h"

namespace gcl {

void comment_tokenize(const String &p_source, Vector<Token> &r_tokens) {
	r_tokens.clear();

	const int L = p_source.length();
	int i = 0;
	int line = 1;
	int col = 1;

	while (i < L) {
		const CharType c = p_source[i];

		/* Blok yorum: #| ... |# (ic ice acilabilir) */
		if (c == '#' && i + 1 < L && p_source[i + 1] == '|') {
			Token tok;
			tok.kind = TokenKind::COMMENT_BLOCK;
			tok.line = line;
			tok.col = col;

			String text;
			text += c;
			text += p_source[i + 1];
			i += 2;
			col += 2;

			int depth = 1;
			while (i < L && depth > 0) {
				if (p_source[i] == '#' && i + 1 < L && p_source[i + 1] == '|') {
					depth++;
					text += p_source[i];
					text += p_source[i + 1];
					i += 2;
					col += 2;
				} else if (p_source[i] == '|' && i + 1 < L && p_source[i + 1] == '#') {
					depth--;
					text += p_source[i];
					text += p_source[i + 1];
					i += 2;
					col += 2;
				} else {
					if (p_source[i] == '\n') {
						line++;
						col = 1;
					} else {
						col++;
					}
					text += p_source[i];
					i++;
				}
			}

			tok.text = text;
			r_tokens.push_back(tok);
			continue;
		}

		/* Satir yorumu: # ... (satir sonuna kadar)
		   ISTISNA: #extern "dll" / #register / #include direktifleri yorum
		   DEGILDIR; strip_comments onlari silmemeli. (executor/loader bunlari
		   yorum temizlemeden once cozer.) */
		if (c == '#') {
			bool is_directive = p_source.find("#extern", i) == i ||
					p_source.find("#register", i) == i ||
					p_source.find("#include", i) == i;

			Token tok;
			if (is_directive) {
				tok.kind = TokenKind::CODE_TEXT;
			} else {
				tok.kind = TokenKind::COMMENT_LINE;
			}
			tok.line = line;
			tok.col = col;

			String text;
			while (i < L && p_source[i] != '\n') {
				text += p_source[i];
				i++;
				col++;
			}

			tok.text = text;
			r_tokens.push_back(tok);
			continue;
		}

		/* Kod / yeni satir: comment disinda kalan icerik */
		if (c == '\n') {
			Token tok;
			tok.kind = TokenKind::NEWLINE;
			tok.text = "\n";
			tok.line = line;
			tok.col = col;
			r_tokens.push_back(tok);
			line++;
			col = 1;
			i++;
			continue;
		}

		/* Yorum disindaki kod: en yakin yoruma / satir sonuna kadar bir yigin */
		Token tok;
		tok.kind = TokenKind::CODE_TEXT;
		tok.line = line;
		tok.col = col;

		String text;
		while (i < L && p_source[i] != '\n' && p_source[i] != '#') {
			text += p_source[i];
			i++;
			col++;
		}

		tok.text = text;
		r_tokens.push_back(tok);
	}

	Token eof;
	eof.kind = TokenKind::EOF_TOKEN;
	eof.line = line;
	eof.col = col;
	r_tokens.push_back(eof);
}

String strip_comments(const String &p_source) {
	Vector<Token> tokens;
	comment_tokenize(p_source, tokens);

	StringBuilder out;
	for (int i = 0; i < tokens.size(); i++) {
		const Token &tok = tokens[i];
		if (tok.kind == TokenKind::COMMENT_LINE || tok.kind == TokenKind::COMMENT_BLOCK) {
			continue; /* yorumlari atla */
		}
		if (tok.kind == TokenKind::EOF_TOKEN) {
			break;
		}
		out += tok.text;
	}

	return out.as_string();
}

} // namespace gcl
