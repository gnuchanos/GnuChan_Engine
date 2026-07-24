/**************************************************************************/
/*  gdscript_extend_parser.h                                              */
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

#ifndef GDSCRIPT_EXTEND_PARSER_H
#define GDSCRIPT_EXTEND_PARSER_H

#include "../gdscript_parser.h"
#include "core/variant.h"
#include "lsp.hpp"

#ifndef LINE_NUMBER_TO_INDEX
#define LINE_NUMBER_TO_INDEX(p_line) ((p_line)-1)
#endif

#ifndef SYMBOL_SEPERATOR
#define SYMBOL_SEPERATOR "::"
#endif

#ifndef JOIN_SYMBOLS
#define JOIN_SYMBOLS(p_path, name) ((p_path) + SYMBOL_SEPERATOR + (name))
#endif

typedef HashMap<String, const lsp::DocumentSymbol *> ClassMembers;

class ExtendGDScriptParser : public GDScriptParser {
	String path;
	Vector<String> lines;

	lsp::DocumentSymbol class_symbol;
	Vector<lsp::Diagnostic> diagnostics;
	List<lsp::DocumentLink> document_links;
	ClassMembers members;
	HashMap<String, ClassMembers> inner_classes;

	void update_diagnostics();

	void update_symbols();
	void update_document_links(const String &p_code);
	void parse_class_symbol(const GDScriptParser::ClassNode *p_class, lsp::DocumentSymbol &r_symbol);
	void parse_function_symbol(const GDScriptParser::FunctionNode *p_func, lsp::DocumentSymbol &r_symbol);

	Dictionary dump_function_api(const GDScriptParser::FunctionNode *p_func) const;
	Dictionary dump_class_api(const GDScriptParser::ClassNode *p_class) const;

	String parse_documentation(int p_line, bool p_docs_down = false);
	const lsp::DocumentSymbol *search_symbol_defined_at_line(int p_line, const lsp::DocumentSymbol &p_parent) const;

	Array member_completions;

public:
	_FORCE_INLINE_ const String &get_path() const { return path; }
	_FORCE_INLINE_ const Vector<String> &get_lines() const { return lines; }
	_FORCE_INLINE_ const lsp::DocumentSymbol &get_symbols() const { return class_symbol; }
	_FORCE_INLINE_ const Vector<lsp::Diagnostic> &get_diagnostics() const { return diagnostics; }
	_FORCE_INLINE_ const ClassMembers &get_members() const { return members; }
	_FORCE_INLINE_ const HashMap<String, ClassMembers> &get_inner_classes() const { return inner_classes; }

	Error get_left_function_call(const lsp::Position &p_position, lsp::Position &r_func_pos, int &r_arg_index) const;

	String get_text_for_completion(const lsp::Position &p_cursor) const;
	String get_text_for_lookup_symbol(const lsp::Position &p_cursor, const String &p_symbol = "", bool p_func_required = false) const;
	String get_identifier_under_position(const lsp::Position &p_position, Vector2i &p_offset) const;
	String get_uri() const;

	const lsp::DocumentSymbol *get_symbol_defined_at_line(int p_line) const;
	const lsp::DocumentSymbol *get_member_symbol(const String &p_name, const String &p_subclass = "") const;
	const List<lsp::DocumentLink> &get_document_links() const;

	const Array &get_member_completions();
	Dictionary generate_api() const;

	Error parse(const String &p_code, const String &p_path);
};

#endif // GDSCRIPT_EXTEND_PARSER_H
