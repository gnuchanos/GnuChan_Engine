/**************************************************************************/
/*  comment.cpp                                                           */
/**************************************************************************/
/*  GCL AST - Comment modulu.                                             */
/*  Lexer'in comment tokenini AST comment dugumune cevirir.               */
/**************************************************************************/

#include "ast.h"

namespace gcl {

Node *comment_build(const Token &p_token) {
	Node *node = memnew(Node);

	if (p_token.kind == TokenKind::COMMENT_BLOCK) {
		node->kind = NodeKind::COMMENT_BLOCK;
	} else {
		node->kind = NodeKind::COMMENT_LINE;
	}

	node->text = p_token.text;
	node->line = p_token.line;
	node->col = p_token.col;

	return node;
}

} // namespace gcl
