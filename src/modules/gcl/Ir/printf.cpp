/**************************************************************************/
/*  printf.cpp                                                            */
/**************************************************************************/
/*  GCL IR - Printf modulu.                                               */
/*  printf format kuralindan IR PRINTF talimati uretir.                   */
/**************************************************************************/

#include "ir.h"

namespace gcl {

IrInstruction printf_lower(const PrintfFormatRule *p_rule) {
	IrInstruction instr;
	instr.op = IrOp::PRINTF;
	instr.text = String(p_rule->type_name) + " -> \"" + String(p_rule->format) + "\"";
	return instr;
}

} // namespace gcl
