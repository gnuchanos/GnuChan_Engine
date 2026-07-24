/**************************************************************************/
/*  script_class_parser.h                                                 */
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

#ifndef SCRIPT_CLASS_PARSER_H
#define SCRIPT_CLASS_PARSER_H

#include "core/ustring.h"
#include "core/variant.h"
#include "core/vector.h"

class ScriptClassParser {
public:
	struct NameDecl {
		enum Type {
			NAMESPACE_DECL,
			CLASS_DECL,
			STRUCT_DECL
		};

		String name;
		Type type;
	};

	struct ClassDecl {
		String name;
		String namespace_;
		Vector<String> base;
		bool nested;
		bool has_script_attr;
	};

private:
	String code;
	int idx;
	int line;
	String error_str;
	bool error;
	Variant value;

	Vector<ClassDecl> classes;

	enum Token {
		TK_BRACKET_OPEN,
		TK_BRACKET_CLOSE,
		TK_CURLY_BRACKET_OPEN,
		TK_CURLY_BRACKET_CLOSE,
		TK_PERIOD,
		TK_COLON,
		TK_COMMA,
		TK_SYMBOL,
		TK_IDENTIFIER,
		TK_STRING,
		TK_NUMBER,
		TK_OP_LESS,
		TK_OP_GREATER,
		TK_EOF,
		TK_ERROR,
		TK_MAX
	};

	static const char *token_names[TK_MAX];
	static String get_token_name(Token p_token);

	Token get_token();

	Error _skip_generic_type_params();

	Error _parse_type_full_name(String &r_full_name);
	Error _parse_class_base(Vector<String> &r_base);
	Error _parse_type_constraints();
	Error _parse_namespace_name(String &r_name, int &r_curly_stack);

public:
	Error parse(const String &p_code);
	Error parse_file(const String &p_filepath);

	String get_error();

	Vector<ClassDecl> get_classes();
};

#endif // SCRIPT_CLASS_PARSER_H
