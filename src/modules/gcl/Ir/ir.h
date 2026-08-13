/**************************************************************************/
/*  ir.h                                                                  */
/**************************************************************************/
/*  GCL IR - Duz (linear) ara temsil talimatlari.                          */
/*  SADECE bildirim; implementasyon .cpp tarafindadir.                    */
/**************************************************************************/

#ifndef GCL_IR_H
#define GCL_IR_H

#include "core/ustring.h"
#include "core/vector.h"

#include "../Ast/ast.h"
#include "../Parser/parser.h"

namespace gcl {

/* IR talimat turleri. */
enum class IrOp : uint8_t {
	COMMENT_LINE,  /* # ... */
	COMMENT_BLOCK, /* #| ... |# */
	FUNCTION,      /* void Name() { govde } */
	DECL_NUMBER,   /* sayi tipi bildirimi */
	PRINTF,        /* printf cagrisi */
	NOP,
};

/* Tek IR talimati. */
struct IrInstruction {
	IrOp op;
	String text;
	int line;
	int col;

	IrInstruction() :
			op(IrOp::NOP),
			line(1),
			col(1) {
	}
};

/* Comment modulu: AST comment dugumunden IR comment talimati uretir. */
IrInstruction comment_lower(const Node &p_node);

/* Function modulu: FunctionNode'dan IR FUNCTION talimati uretir. */
IrInstruction function_lower(const FunctionNode &p_node);

/* Number modulu: sayi tipinden IR DECL_NUMBER talimati uretir. */
IrInstruction number_lower(const NumberTypeInfo *p_info);

/* Printf modulu: printf format kuralindan IR PRINTF talimati uretir. */
IrInstruction printf_lower(const PrintfFormatRule *p_rule);

} // namespace gcl

#endif // GCL_IR_H
