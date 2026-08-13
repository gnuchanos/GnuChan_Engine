/**************************************************************************/
/*  number.cpp                                                            */
/**************************************************************************/
/*  GCL Parser - Number modulu.                                           */
/*  GCL sayi tipleri + C++ karsiliklari (kullanici tanimli tablo).        */
/*  ccstdint / __int128 / _Float16 / __float128 dahil.                    */
/**************************************************************************/

#include "parser.h"

namespace gcl {

static const NumberTypeInfo number_types[] = {
	/* signed tam sayilar */
	{ "int8", "std::int8_t" },
	{ "int16", "std::int16_t" },
	{ "int32", "std::int32_t" },
	{ "int64", "std::int64_t" },
	{ "int128", "__int128" },

	/* C tarzi signed */
	{ "short", "short" },
	{ "int", "int" },
	{ "long", "long" },
	{ "long long", "long long" },

	/* unsigned tam sayilar */
	{ "uint8", "std::uint8_t" },
	{ "uint16", "std::uint16_t" },
	{ "uint32", "std::uint32_t" },
	{ "uint64", "std::uint64_t" },
	{ "uint128", "unsigned __int128" },

	/* C tarzi unsigned */
	{ "unsigned short", "unsigned short" },
	{ "unsigned int", "unsigned int" },
	{ "unsigned long", "unsigned long" },
	{ "unsigned long long", "unsigned long long" },

	/* kayan nokta */
	{ "float16", "_Float16" },
	{ "float32", "float" },
	{ "float64", "double" },
	{ "float128", "__float128" },

	/* C tarzi kayan nokta */
	{ "float", "float" },
	{ "double", "double" },
	{ "long double", "long double" },
};

const NumberTypeInfo *number_type_get(int p_index) {
	if (p_index < 0 || p_index >= (int)(sizeof(number_types) / sizeof(number_types[0]))) {
		return nullptr;
	}
	return &number_types[p_index];
}

int number_type_count() {
	return (int)(sizeof(number_types) / sizeof(number_types[0]));
}

} // namespace gcl
