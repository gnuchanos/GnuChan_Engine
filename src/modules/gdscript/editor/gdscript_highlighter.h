/**************************************************************************/
/*  gdscript_highlighter.h                                                */
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

#ifndef GDSCRIPT_HIGHLIGHTER_H
#define GDSCRIPT_HIGHLIGHTER_H

#include "scene/gui/text_edit.h"

class GDScriptSyntaxHighlighter : public SyntaxHighlighter {
private:
	enum Type {
		NONE,
		REGION,
		NODE_PATH,
		SYMBOL,
		NUMBER,
		FUNCTION,
		KEYWORD,
		MEMBER,
		IDENTIFIER,
		TYPE,
	};

	// colours
	Color font_color;
	Color symbol_color;
	Color function_color;
	Color function_definition_color;
	Color built_in_type_color;
	Color number_color;
	Color member_color;
	Color node_path_color;
	Color type_color;

public:
	static SyntaxHighlighter *create();

	virtual void _update_cache();
	virtual Map<int, TextEdit::HighlighterInfo> _get_line_syntax_highlighting(int p_line);

	virtual String get_name() const;
	virtual List<String> get_supported_languages();
};

#endif // GDSCRIPT_HIGHLIGHTER_H
