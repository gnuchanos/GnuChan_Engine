/**************************************************************************/
/*  navigation_2d.h                                                       */
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

#ifndef NAVIGATION_2D_H
#define NAVIGATION_2D_H

#include "scene/2d/navigation_polygon.h"
#include "scene/2d/node_2d.h"

class Navigation2D : public Node2D {
	GDCLASS(Navigation2D, Node2D);

	RID map;
	real_t cell_size = 1.0;
	real_t edge_connection_margin = 1.0;
	uint32_t navigation_layers = 1;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	RID get_rid() const {
		return map;
	}

	void set_cell_size(float p_cell_size);
	float get_cell_size() const {
		return cell_size;
	}

	void set_navigation_layers(uint32_t p_navigation_layers);
	uint32_t get_navigation_layers() const;

	void set_edge_connection_margin(float p_edge_connection_margin);
	float get_edge_connection_margin() const {
		return edge_connection_margin;
	}

	Vector<Vector2> get_simple_path(const Vector2 &p_start, const Vector2 &p_end, bool p_optimize = true) const;
	Vector2 get_closest_point(const Vector2 &p_point) const;
	RID get_closest_point_owner(const Vector2 &p_point) const;

	virtual String get_configuration_warning() const;

	Navigation2D();
	~Navigation2D();
};

#endif // NAVIGATION_2D_H
