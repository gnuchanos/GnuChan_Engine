/**************************************************************************/
/*  instance_placeholder.h                                                */
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

#ifndef INSTANCE_PLACEHOLDER_H
#define INSTANCE_PLACEHOLDER_H

#include "scene/main/node.h"

class PackedScene;

class InstancePlaceholder : public Node {
	GDCLASS(InstancePlaceholder, Node);

	String path;
	struct PropSet {
		StringName name;
		Variant value;
	};

	List<PropSet> stored_values;

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void _bind_methods();

public:
	void set_instance_path(const String &p_name);
	String get_instance_path() const;

	Dictionary get_stored_values(bool p_with_order = false);

	Node *create_instance(bool p_replace = false, const Ref<PackedScene> &p_custom_scene = Ref<PackedScene>());
	void replace_by_instance(const Ref<PackedScene> &p_custom_scene = Ref<PackedScene>());

	InstancePlaceholder();
};

#endif // INSTANCE_PLACEHOLDER_H
