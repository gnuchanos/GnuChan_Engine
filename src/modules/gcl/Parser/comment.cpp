/**************************************************************************/
/*  comment.cpp                                                           */
/**************************************************************************/
/*  GCL Parser - Comment modulu.                                          */
/*  Lexer'in urettigi comment tokenlarini tuketir (parse eder).          */
/**************************************************************************/

#include "parser.h"

namespace gcl {

void comment_parse(const Vector<Token> &p_tokens, int &r_index) {
	const int L = p_tokens.size();

	while (r_index < L) {
		const Token &tok = p_tokens[r_index];

		if (tok.kind == TokenKind::COMMENT_LINE || tok.kind == TokenKind::COMMENT_BLOCK) {
			r_index++;
			continue;
		}
		if (tok.kind == TokenKind::NEWLINE) {
			r_index++;
			continue;
		}
		break;
	}
}

} // namespace gcl
