/**************************************************************************/
/*  editor_property_name_processor.h                                      */
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

#ifndef EDITOR_PROPERTY_NAME_PROCESSOR_H
#define EDITOR_PROPERTY_NAME_PROCESSOR_H

#include "scene/main/node.h"

class EditorPropertyNameProcessor : public Node {
	GDCLASS(EditorPropertyNameProcessor, Node);

	static EditorPropertyNameProcessor *singleton;

	mutable Map<String, String> capitalize_string_cache;
	Map<String, String> capitalize_string_remaps;
	LocalVector<String> stop_words; // Exceptions that shouldn't be capitalized.

	// Capitalizes property path segments.
	String _capitalize_name(const String &p_name) const;

public:
	// Matches `interface/inspector/capitalize_properties` editor setting.
	enum Style {
		STYLE_RAW,
		STYLE_CAPITALIZED,
		STYLE_LOCALIZED,
	};

	static EditorPropertyNameProcessor *get_singleton() { return singleton; }

	static Style get_default_inspector_style();
	static Style get_settings_style();
	static Style get_tooltip_style(Style p_style);

	static bool is_localization_available();

	// Turns property path segment into the given style.
	String process_name(const String &p_name, Style p_style) const;

	EditorPropertyNameProcessor();
	~EditorPropertyNameProcessor();
};

#endif // EDITOR_PROPERTY_NAME_PROCESSOR_H
