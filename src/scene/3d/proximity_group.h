/**************************************************************************/
/*  proximity_group.h                                                     */
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

#ifndef PROXIMITY_GROUP_H
#define PROXIMITY_GROUP_H

#include "spatial.h"

class ProximityGroup : public Spatial {
	GDCLASS(ProximityGroup, Spatial);

public:
	enum DispatchMode {
		MODE_PROXY,
		MODE_SIGNAL,
	};

private:
	Map<StringName, uint32_t> groups;

	String group_name;
	DispatchMode dispatch_mode;
	Vector3 grid_radius;

	float cell_size;
	uint32_t group_version;

	void _clear_groups();
	void _update_groups();
	void _add_groups(int *p_cell, String p_base, int p_depth);
	void _new_group(StringName p_name);

	void _proximity_group_broadcast(String p_method, Variant p_parameters);

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	void set_group_name(const String &p_group_name);
	String get_group_name() const;

	void set_dispatch_mode(DispatchMode p_mode);
	DispatchMode get_dispatch_mode() const;

	void set_grid_radius(const Vector3 &p_radius);
	Vector3 get_grid_radius() const;

	void broadcast(String p_method, Variant p_parameters);

	ProximityGroup();
	~ProximityGroup() {}
};

VARIANT_ENUM_CAST(ProximityGroup::DispatchMode);

#endif // PROXIMITY_GROUP_H
