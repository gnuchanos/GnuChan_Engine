/**************************************************************************/
/*  regex.h                                                               */
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

#ifndef REGEX_H
#define REGEX_H

#include "core/array.h"
#include "core/dictionary.h"
#include "core/map.h"
#include "core/reference.h"
#include "core/ustring.h"
#include "core/vector.h"

class RegExMatch : public Reference {
	GDCLASS(RegExMatch, Reference);

	struct Range {
		int start;
		int end;
	};

	String subject;
	Vector<Range> data;
	Map<String, int> names;

	friend class RegEx;

protected:
	static void _bind_methods();

	int _find(const Variant &p_name) const;

public:
	String get_subject() const;
	int get_group_count() const;
	Dictionary get_names() const;

	Array get_strings() const;
	String get_string(const Variant &p_name) const;
	int get_start(const Variant &p_name) const;
	int get_end(const Variant &p_name) const;
};

class RegEx : public Reference {
	GDCLASS(RegEx, Reference);

	void *general_ctx;
	void *code;
	String pattern;

	void _pattern_info(uint32_t what, void *where) const;

protected:
	static void _bind_methods();

public:
	void clear();
	Error compile(const String &p_pattern);

	Ref<RegExMatch> search(const String &p_subject, int p_offset = 0, int p_end = -1) const;
	Array search_all(const String &p_subject, int p_offset = 0, int p_end = -1) const;
	String sub(const String &p_subject, const String &p_replacement, bool p_all = false, int p_offset = 0, int p_end = -1) const;

	bool is_valid() const;
	String get_pattern() const;
	int get_group_count() const;
	Array get_names() const;

	RegEx();
	RegEx(const String &p_pattern);
	~RegEx();
};

#endif // REGEX_H
