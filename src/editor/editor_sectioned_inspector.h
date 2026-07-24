/**************************************************************************/
/*  editor_sectioned_inspector.h                                          */
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

#ifndef EDITOR_SECTIONED_INSPECTOR_H
#define EDITOR_SECTIONED_INSPECTOR_H

#include "editor/editor_inspector.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tree.h"

class SectionedInspectorFilter;

class SectionedInspector : public HSplitContainer {
	GDCLASS(SectionedInspector, HSplitContainer);

	ObjectID obj;

	Tree *sections;
	SectionedInspectorFilter *filter;

	Map<String, TreeItem *> section_map;
	EditorInspector *inspector;
	LineEdit *search_box;

	String selected_category;

	static void _bind_methods();
	void _section_selected();

	void _search_changed(const String &p_what);
	bool _property_path_matches(const String &p_property_path, const String &p_filter, EditorPropertyNameProcessor::Style p_style);

protected:
	void _notification(int p_what);

public:
	void register_search_box(LineEdit *p_box);
	EditorInspector *get_inspector();
	void edit(Object *p_object);
	String get_full_item_path(const String &p_item);

	void set_current_section(const String &p_section);
	String get_current_section() const;

	void update_category_list();

	SectionedInspector();
	~SectionedInspector();
};

#endif // EDITOR_SECTIONED_INSPECTOR_H
