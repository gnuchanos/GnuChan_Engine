/**************************************************************************/
/*  doc_data.h                                                            */
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

#ifndef DOC_DATA_H
#define DOC_DATA_H

#include "core/io/xml_parser.h"
#include "core/map.h"
#include "core/variant.h"

class DocData {
public:
	struct ArgumentDoc {
		String name;
		String type;
		String enumeration;
		String default_value;
	};

	struct MethodDoc {
		String name;
		String return_type;
		String return_enum;
		String qualifiers;
		String description;
		Vector<ArgumentDoc> arguments;
		bool operator<(const MethodDoc &p_md) const {
			return name < p_md.name;
		}
	};

	struct ConstantDoc {
		String name;
		String value;
		bool is_value_valid;
		String enumeration;
		String description;
	};

	struct PropertyDoc {
		String name;
		String type;
		String enumeration;
		String description;
		String setter, getter;
		String default_value;
		bool overridden;
		String overrides;
		bool operator<(const PropertyDoc &p_prop) const {
			return name < p_prop.name;
		}
		PropertyDoc() {
			overridden = false;
		}
	};

	struct ThemeItemDoc {
		String name;
		String type;
		String data_type;
		String description;
		String default_value;
		bool operator<(const ThemeItemDoc &p_theme_item) const {
			// First sort by the data type, then by name.
			if (data_type == p_theme_item.data_type) {
				return name < p_theme_item.name;
			}
			return data_type < p_theme_item.data_type;
		}
	};

	struct TutorialDoc {
		String link;
		String title;
	};

	struct ClassDoc {
		String name;
		String inherits;
		String category;
		String brief_description;
		String description;
		Vector<TutorialDoc> tutorials;
		Vector<MethodDoc> methods;
		Vector<MethodDoc> signals;
		Vector<ConstantDoc> constants;
		Vector<PropertyDoc> properties;
		Vector<ThemeItemDoc> theme_properties;
	};

	String version;

	Map<String, ClassDoc> class_list;
	Error _load(Ref<XMLParser> parser);

public:
	void merge_from(const DocData &p_data);
	void remove_from(const DocData &p_data);
	void generate(bool p_basic_types = false);
	Error load_classes(const String &p_dir);
	static Error erase_classes(const String &p_dir);
	Error save_classes(const String &p_default_path, const Map<String, String> &p_class_path);

	Error load_compressed(const uint8_t *p_data, int p_compressed_size, int p_uncompressed_size);
};

#endif // DOC_DATA_H
