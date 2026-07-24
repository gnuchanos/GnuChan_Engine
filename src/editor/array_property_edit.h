/**************************************************************************/
/*  array_property_edit.h                                                 */
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

#ifndef ARRAY_PROPERTY_EDIT_H
#define ARRAY_PROPERTY_EDIT_H

#include "scene/main/node.h"

class ArrayPropertyEdit : public Reference {
	GDCLASS(ArrayPropertyEdit, Reference);

	int page;
	ObjectID obj;
	StringName property;
	String vtypes;
	String subtype_hint_string;
	PropertyHint subtype_hint;
	Variant::Type subtype;
	Variant get_array() const;
	Variant::Type default_type;

	void _notif_change();
	void _notif_changev(const String &p_v);
	void _set_size(int p_size);
	void _set_value(int p_idx, const Variant &p_value);

	bool _dont_undo_redo();

protected:
	static void _bind_methods();
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	void edit(Object *p_obj, const StringName &p_prop, const String &p_hint_string, Variant::Type p_deftype);

	Node *get_node();

	ArrayPropertyEdit();
};

#endif // ARRAY_PROPERTY_EDIT_H
