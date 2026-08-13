/**************************************************************************/
/*  ast.h                                                                 */
/**************************************************************************/
/*  GCL AST - Parser'in urettigi agac dugumleri.                           */
/*  SADECE bildirim; implementasyon .cpp tarafindadir.                    */
/**************************************************************************/

#ifndef GCL_AST_H
#define GCL_AST_H

#include "core/ustring.h"
#include "core/vector.h"

#include "../Lexer/lexer.h"

namespace gcl {

/* AST dugum turleri. */
enum class NodeKind : uint8_t {
	COMMENT_LINE,  /* # ... */
	COMMENT_BLOCK, /* #| ... |# */
	FUNCTION,      /* void Name() { } */
	PROGRAM,
};

/* Fonksiyon dugumu: void Name( ) { govde } */
struct FunctionNode {
	String name;
	int line;
	int col;
};

/* Ortak AST dugumu. */
struct Node {
	NodeKind kind;
	String text;
	int line;
	int col;

	Node() :
			kind(NodeKind::PROGRAM),
			line(1),
			col(1) {
	}
};

/* Comment modulu: comment tokenlarindan AST comment dugumu uretir. */
Node *comment_build(const Token &p_token);

/* Function modulu: parser bulgusuyla FunctionNode doldurur. */
void function_build(const String &p_name, int p_line, int p_col, FunctionNode &r_node);

} // namespace gcl

#endif // GCL_AST_H
