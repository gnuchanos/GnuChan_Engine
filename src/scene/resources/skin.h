/**************************************************************************/
/*  skin.h                                                                */
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

#ifndef SKIN_H
#define SKIN_H

#include "core/resource.h"

class Skin : public Resource {
	GDCLASS(Skin, Resource)

	struct Bind {
		int bone = -1;
		StringName name;
		Transform pose;
	};

	Vector<Bind> binds;

	Bind *binds_ptr;
	int bind_count;

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void _bind_methods();

public:
	void set_bind_count(int p_size);
	inline int get_bind_count() const { return bind_count; }

	void add_bind(int p_bone, const Transform &p_pose);
	void add_named_bind(const String &p_name, const Transform &p_pose);

	void set_bind_bone(int p_index, int p_bone);
	void set_bind_pose(int p_index, const Transform &p_pose);
	void set_bind_name(int p_index, const StringName &p_name);

	inline int get_bind_bone(int p_index) const {
#ifdef DEBUG_ENABLED
		ERR_FAIL_INDEX_V(p_index, bind_count, -1);
#endif
		return binds_ptr[p_index].bone;
	}

	inline StringName get_bind_name(int p_index) const {
#ifdef DEBUG_ENABLED
		ERR_FAIL_INDEX_V(p_index, bind_count, StringName());
#endif
		return binds_ptr[p_index].name;
	}

	inline Transform get_bind_pose(int p_index) const {
#ifdef DEBUG_ENABLED
		ERR_FAIL_INDEX_V(p_index, bind_count, Transform());
#endif
		return binds_ptr[p_index].pose;
	}

	void clear_binds();

	Skin();
};

#endif // SKIN_H
