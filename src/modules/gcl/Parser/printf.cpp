/**************************************************************************/
/*  printf.cpp                                                            */
/**************************************************************************/
/*  GCL Parser - Printf modulu.                                           */
/*  GCL sayi tipleri + printf format karsiliklari (kullanici tablo).      */
/*  int128/uint128/float128: C'de karsiligi olan en yakin format.         */
/**************************************************************************/

#include "parser.h"

namespace gcl {

static const PrintfFormatRule printf_formats[] = {
	/* signed tam sayilar */
	{ "int8", "%d" },
	{ "int16", "%d" },
	{ "int32", "%d" },
	{ "int64", "%lld" },
	{ "int128", "%lld" },

	/* C tarzi signed */
	{ "short", "%hd" },
	{ "int", "%d" },
	{ "long", "%ld" },
	{ "long long", "%lld" },

	/* unsigned tam sayilar */
	{ "uint8", "%u" },
	{ "uint16", "%u" },
	{ "uint32", "%u" },
	{ "uint64", "%llu" },
	{ "uint128", "%llu" },

	/* C tarzi unsigned */
	{ "unsigned short", "%hu" },
	{ "unsigned int", "%u" },
	{ "unsigned long", "%lu" },
	{ "unsigned long long", "%llu" },

	/* kayan nokta */
	{ "float16", "%f" },
	{ "float32", "%f" },
	{ "float64", "%f" },
	{ "float128", "%Lf" },

	/* C tarzi kayan nokta */
	{ "float", "%f" },
	{ "double", "%f" },
	{ "long double", "%Lf" },
};

const PrintfFormatRule *printf_format_get(int p_index) {
	if (p_index < 0 || p_index >= (int)(sizeof(printf_formats) / sizeof(printf_formats[0]))) {
		return nullptr;
	}
	return &printf_formats[p_index];
}

int printf_format_count() {
	return (int)(sizeof(printf_formats) / sizeof(printf_formats[0]));
}

} // namespace gcl
