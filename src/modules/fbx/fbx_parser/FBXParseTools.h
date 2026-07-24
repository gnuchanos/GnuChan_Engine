/**************************************************************************/
/*  FBXParseTools.h                                                       */
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

#ifndef FBXPARSETOOLS_H
#define FBXPARSETOOLS_H

#include "core/error_macros.h"
#include "core/ustring.h"

#include <stdint.h>
#include <algorithm>
#include <locale>

template <class char_t>
inline bool IsNewLine(char_t c) {
	return c == '\n' || c == '\r';
}
template <class char_t>
inline bool IsSpace(char_t c) {
	return (c == (char_t)' ' || c == (char_t)'\t');
}

template <class char_t>
inline bool IsSpaceOrNewLine(char_t c) {
	return IsNewLine(c) || IsSpace(c);
}

template <class char_t>
inline bool IsLineEnd(char_t c) {
	return (c == (char_t)'\r' || c == (char_t)'\n' || c == (char_t)'\0' || c == (char_t)'\f');
}

// ------------------------------------------------------------------------------------
// Special version of the function, providing higher accuracy and safety
// It is mainly used by fast_atof to prevent ugly and unwanted integer overflows.
// ------------------------------------------------------------------------------------
inline uint64_t strtoul10_64(const char *in, bool &errored, const char **out = nullptr, unsigned int *max_inout = nullptr) {
	unsigned int cur = 0;
	uint64_t value = 0;

	errored = *in < '0' || *in > '9';
	ERR_FAIL_COND_V_MSG(errored, 0, "The string cannot be converted parser error");

	for (;;) {
		if (*in < '0' || *in > '9') {
			break;
		}

		const uint64_t new_value = (value * (uint64_t)10) + ((uint64_t)(*in - '0'));

		// numeric overflow, we rely on you
		if (new_value < value) {
			//WARN_PRINT( "Converting the string \" " + in + " \" into a value resulted in overflow." );
			return 0;
		}

		value = new_value;

		++in;
		++cur;

		if (max_inout && *max_inout == cur) {
			if (out) { /* skip to end */
				while (*in >= '0' && *in <= '9') {
					++in;
				}
				*out = in;
			}

			return value;
		}
	}
	if (out) {
		*out = in;
	}

	if (max_inout) {
		*max_inout = cur;
	}

	return value;
}

#endif // FBXPARSETOOLS_H
