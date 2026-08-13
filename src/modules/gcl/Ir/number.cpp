/**************************************************************************/
/*  number.cpp                                                            */
/**************************************************************************/
/*  GCL IR - Number modulu.                                               */
/*  Sayi tipi bilgisinden IR DECL_NUMBER talimati uretir.                 */
/**************************************************************************/

#include "ir.h"

namespace gcl {

IrInstruction number_lower(const NumberTypeInfo *p_info) {
	IrInstruction instr;
	instr.op = IrOp::DECL_NUMBER;
	instr.text = String(p_info->name) + " = " + String(p_info->cpp_type);
	return instr;
}

} // namespace gcl
