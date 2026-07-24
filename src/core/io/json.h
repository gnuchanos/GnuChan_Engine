/**************************************************************************/
/*  json.h                                                                */
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

#ifndef JSON_H
#define JSON_H

#include "core/variant.h"

class JSON {
	enum TokenType {
		TK_CURLY_BRACKET_OPEN,
		TK_CURLY_BRACKET_CLOSE,
		TK_BRACKET_OPEN,
		TK_BRACKET_CLOSE,
		TK_IDENTIFIER,
		TK_STRING,
		TK_NUMBER,
		TK_COLON,
		TK_COMMA,
		TK_EOF,
		TK_MAX
	};

	enum Expecting {

		EXPECT_OBJECT,
		EXPECT_OBJECT_KEY,
		EXPECT_COLON,
		EXPECT_OBJECT_VALUE,
	};

	struct Token {
		TokenType type;
		Variant value;
	};

	static const char *tk_name[TK_MAX];

	static String _print_var(const Variant &p_var, const String &p_indent, int p_cur_indent, bool p_sort_keys, Set<const void *> &p_markers);

	static Error _get_token(const CharType *p_str, int &index, int p_len, Token &r_token, int &line, String &r_err_str);
	static Error _parse_value(Variant &value, Token &token, const CharType *p_str, int &index, int p_len, int &line, String &r_err_str);
	static Error _parse_array(Array &array, const CharType *p_str, int &index, int p_len, int &line, String &r_err_str);
	static Error _parse_object(Dictionary &object, const CharType *p_str, int &index, int p_len, int &line, String &r_err_str);

public:
	static String print(const Variant &p_var, const String &p_indent = "", bool p_sort_keys = true);
	static Error parse(const String &p_json, Variant &r_ret, String &r_err_str, int &r_err_line);
};

#endif // JSON_H
