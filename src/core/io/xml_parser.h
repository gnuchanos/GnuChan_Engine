/**************************************************************************/
/*  xml_parser.h                                                          */
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

#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "core/os/file_access.h"
#include "core/reference.h"
#include "core/ustring.h"
#include "core/vector.h"

/*
  Based on irrXML (see their zlib license). Added mainly for compatibility with their Collada loader.
*/

class XMLParser : public Reference {
	GDCLASS(XMLParser, Reference);

public:
	//! Enumeration of all supported source text file formats
	enum SourceFormat {
		SOURCE_ASCII,
		SOURCE_UTF8,
		SOURCE_UTF16_BE,
		SOURCE_UTF16_LE,
		SOURCE_UTF32_BE,
		SOURCE_UTF32_LE
	};

	enum NodeType {
		NODE_NONE,
		NODE_ELEMENT,
		NODE_ELEMENT_END,
		NODE_TEXT,
		NODE_COMMENT,
		NODE_CDATA,
		NODE_UNKNOWN
	};

private:
	char *data = nullptr;
	char *P = nullptr;
	uint64_t length = 0;
	String node_name;
	bool node_empty = false;
	NodeType node_type = NODE_NONE;
	uint64_t node_offset = 0;

	struct Attribute {
		String name;
		String value;
	};

	Vector<Attribute> attributes;

	bool _set_text(char *start, char *end);
	void _parse_closing_xml_element();
	void _ignore_definition();
	bool _parse_cdata();
	void _parse_comment();
	void _parse_opening_xml_element();
	void _parse_current_node();

	static void _bind_methods();

public:
	Error read();
	NodeType get_node_type();
	String get_node_name() const;
	String get_node_data() const;
	uint64_t get_node_offset() const;
	int get_attribute_count() const;
	String get_attribute_name(int p_idx) const;
	String get_attribute_value(int p_idx) const;
	bool has_attribute(const String &p_name) const;
	String get_attribute_value(const String &p_name) const;
	String get_attribute_value_safe(const String &p_name) const; // do not print error if doesn't exist
	bool is_empty() const;
	int get_current_line() const;

	void skip_section();
	Error seek(uint64_t p_pos);

	Error open(const String &p_path);
	Error open_buffer(const Vector<uint8_t> &p_buffer);

	void close();

	XMLParser();
	~XMLParser();
};

#endif // XML_PARSER_H
