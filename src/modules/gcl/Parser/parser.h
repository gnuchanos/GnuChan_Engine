/**************************************************************************/
/*  parser.h                                                              */
/**************************************************************************/
/*  GCL Parser - Token akisini AST'ye cevirir.                            */
/*  SADECE bildirim; implementasyon .cpp tarafindadir.                    */
/**************************************************************************/

#ifndef GCL_PARSER_H
#define GCL_PARSER_H

#include "core/ustring.h"
#include "core/vector.h"

#include "../Lexer/lexer.h"

namespace gcl {

/* Comment modulu: lexer'in urettigi comment tokenlarini tuketir. */
void comment_parse(const Vector<Token> &p_tokens, int &r_index);

/* Function modulu: void Name() { ... } tanimini bulur. */
bool function_parse(const Vector<Token> &p_tokens, int &r_index, String &r_name, int &r_body_start);

/* Bilinen fonksiyonlar: Ready, Update, PhysicsUpdate. */
bool function_is_known(const String &p_name);

/* Number modulu: GCL sayi tipleri (C++ karsiliklari). */
struct NumberTypeInfo {
	const char *name;     /* "int8" */
	const char *cpp_type; /* "std::int8_t" */
};

const NumberTypeInfo *number_type_get(int p_index);
int number_type_count();

/* Printf modulu: printf format kurallari. */
struct PrintfFormatRule {
	const char *type_name; /* "int8" */
	const char *format;    /* "%d" */
};

const PrintfFormatRule *printf_format_get(int p_index);
int printf_format_count();

/* Extern modulu: "@extern <node>" satirlarini tarar. */
void extern_scan(const String &p_code, Vector<String> &r_nodes);

/* Variable modulu: "<tip> <isim>" tanimlarini tarar (sayi tipleri + int/float/etc). */
void variable_scan(const String &p_code, Vector<String> &r_names);

template <class TParser>
struct ParserResult {
	bool ok;
	String error;
	int error_line;
	int error_col;

	ParserResult() :
			ok(true),
			error_line(0),
			error_col(0) {
	}
};

} // namespace gcl

#endif // GCL_PARSER_H
