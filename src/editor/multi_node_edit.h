/**************************************************************************/
/*  multi_node_edit.h                                                     */
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

#ifndef MULTI_NODE_EDIT_H
#define MULTI_NODE_EDIT_H

#include "scene/main/node.h"

class MultiNodeEdit : public Reference {
	GDCLASS(MultiNodeEdit, Reference);

	List<NodePath> nodes;
	struct PLData {
		int uses;
		PropertyInfo info;
	};

	bool _set_impl(const StringName &p_name, const Variant &p_value, const String &p_field);

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	void clear_nodes();
	void add_node(const NodePath &p_node);

	int get_node_count() const;
	NodePath get_node(int p_index) const;
	StringName get_edited_class_name() const;

	void set_property_field(const StringName &p_property, const Variant &p_value, const String &p_field);

	MultiNodeEdit();
};

#endif // MULTI_NODE_EDIT_H
