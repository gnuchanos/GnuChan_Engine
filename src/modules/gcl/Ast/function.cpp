/**************************************************************************/
/*  function.cpp                                                          */
/**************************************************************************/
/*  GCL AST - Fonksiyon modulu.                                           */
/*  Parser'in buldugu fonksiyonu FunctionNode olarak doldurur.            */
/**************************************************************************/

#include "ast.h"

namespace gcl {

void function_build(const String &p_name, int p_line, int p_col, FunctionNode &r_node) {
	r_node.name = p_name;
	r_node.line = p_line;
	r_node.col = p_col;
}

} // namespace gcl
