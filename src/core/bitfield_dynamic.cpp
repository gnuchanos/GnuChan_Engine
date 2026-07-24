/**************************************************************************/
/*  bitfield_dynamic.cpp                                                  */
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

#include "bitfield_dynamic.h"

#include "core/os/memory.h"

#include <string.h>

void BitFieldDynamic::copy_from(const BitFieldDynamic &p_source) {
	create(p_source.get_num_bits(), false);
	memcpy(_data, p_source.get_data(), p_source.get_num_bytes());
}

void BitFieldDynamic::create(uint32_t p_num_bits, bool p_blank) {
	// first delete any initial
	destroy();

	_num_bits = p_num_bits;
	if (p_num_bits) {
		_num_bytes = (p_num_bits / 8) + 1;
		_data = (uint8_t *)memalloc(_num_bytes);

		if (p_blank) {
			blank(false);
		}
	}
}

void BitFieldDynamic::destroy() {
	if (_data) {
		memfree(_data);
		_data = nullptr;
	}

	_num_bytes = 0;
	_num_bits = 0;
}

void BitFieldDynamic::blank(bool p_set_or_zero) {
	if (p_set_or_zero) {
		memset(_data, 255, _num_bytes);
	} else {
		memset(_data, 0, _num_bytes);
	}
}

void BitFieldDynamic::invert() {
	for (uint32_t n = 0; n < _num_bytes; n++) {
		_data[n] = ~_data[n];
	}
}
