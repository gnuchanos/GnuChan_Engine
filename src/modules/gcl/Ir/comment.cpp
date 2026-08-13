/**************************************************************************/
/*  comment.cpp                                                           */
/**************************************************************************/
/*  GCL IR - Comment modulu.                                              */
/*  AST comment dugumunu IR comment talimati olarak indirir.              */
/**************************************************************************/

#include "ir.h"

namespace gcl {

IrInstruction comment_lower(const Node &p_node) {
	IrInstruction instr;

	if (p_node.kind == NodeKind::COMMENT_BLOCK) {
		instr.op = IrOp::COMMENT_BLOCK;
	} else {
		instr.op = IrOp::COMMENT_LINE;
	}

	instr.text = p_node.text;
	instr.line = p_node.line;
	instr.col = p_node.col;

	return instr;
}

} // namespace gcl
