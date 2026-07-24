/**************************************************************************/
/*  compressed_translation.h                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHAN ENGINE                             */
/*                    https://github.com/gnuchanos/GnuChan_Engine         */
/**************************************************************************/
/* Copyright (C) 2025 GnuChan Engine contributors (see AUTHORS.md).      */
/*                                                                        */
/* This program is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program. If not, see <https://www.gnu.org/licenses/>.  */
/**************************************************************************/

#ifndef COMPRESSED_TRANSLATION_H
#define COMPRESSED_TRANSLATION_H

#include "core/translation.h"

class PHashTranslation : public Translation {
	GDCLASS(PHashTranslation, Translation);

	//this translation uses a sort of modified perfect hash algorithm
	//it requires hashing strings twice and then does a binary search,
	//so it's slower, but at the same time it has an extremely high chance
	//of catching untranslated strings

	//load/store friendly types
	PoolVector<int> hash_table;
	PoolVector<int> bucket_table;
	PoolVector<uint8_t> strings;

	struct Bucket {
		int size;
		uint32_t func;

		struct Elem {
			uint32_t key;
			uint32_t str_offset;
			uint32_t comp_size;
			uint32_t uncomp_size;
		};

		Elem elem[1];
	};

	_FORCE_INLINE_ uint32_t hash(uint32_t d, const char *p_str) const {
		if (d == 0) {
			d = 0x1000193;
		}
		while (*p_str) {
			d = (d * 0x1000193) ^ uint32_t(*p_str);
			p_str++;
		}

		return d;
	}

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

public:
	virtual StringName get_message(const StringName &p_src_text) const; //overridable for other implementations
	void generate(const Ref<Translation> &p_from);

	PHashTranslation();
};

#endif // COMPRESSED_TRANSLATION_H
