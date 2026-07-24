/**************************************************************************/
/*  property_selector.h                                                   */
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

#ifndef PROPERTY_SELECTOR_H
#define PROPERTY_SELECTOR_H

#include "editor/property_editor.h"
#include "editor_help.h"
#include "scene/gui/rich_text_label.h"

class PropertySelector : public ConfirmationDialog {
	GDCLASS(PropertySelector, ConfirmationDialog);

	LineEdit *search_box;
	Tree *search_options;

	void _update_search();

	void _sbox_input(const Ref<InputEvent> &p_ie);

	void _confirmed();
	void _text_changed(const String &p_newtext);

	EditorHelpBit *help_bit;

	bool properties;
	String selected;
	Variant::Type type;
	String base_type;
	ObjectID script;
	Object *instance;
	bool virtuals_only;

	void _item_selected();

	Vector<Variant::Type> type_filter;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void select_method_from_base_type(const String &p_base, const String &p_current = "", bool p_virtuals_only = false);
	void select_method_from_script(const Ref<Script> &p_script, const String &p_current = "");
	void select_method_from_basic_type(Variant::Type p_type, const String &p_current = "");
	void select_method_from_instance(Object *p_instance, const String &p_current = "");

	void select_property_from_base_type(const String &p_base, const String &p_current = "");
	void select_property_from_script(const Ref<Script> &p_script, const String &p_current = "");
	void select_property_from_basic_type(Variant::Type p_type, const String &p_current = "");
	void select_property_from_instance(Object *p_instance, const String &p_current = "");

	void set_type_filter(const Vector<Variant::Type> &p_type_filter);

	PropertySelector();
};

#endif // PROPERTY_SELECTOR_H
