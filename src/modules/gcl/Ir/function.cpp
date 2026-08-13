/**************************************************************************/
/*  function.cpp                                                          */
/**************************************************************************/
/*  GCL IR - Fonksiyon modulu.                                            */
/*  FunctionNode'dan IR FUNCTION talimati uretir.                         */
/**************************************************************************/

#include "ir.h"

namespace gcl {

IrInstruction function_lower(const FunctionNode &p_node) {
	IrInstruction instr;
	instr.op = IrOp::FUNCTION;
	instr.text = p_node.name;
	instr.line = p_node.line;
	instr.col = p_node.col;
	return instr;
}

} // namespace gcl
