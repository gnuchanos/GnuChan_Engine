/**************************************************************************/
/*  convex_polygon_shape.h                                                */
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

#ifndef CONVEX_POLYGON_SHAPE_H
#define CONVEX_POLYGON_SHAPE_H

#include "scene/resources/shape.h"

class ConvexPolygonShape : public Shape {
	GDCLASS(ConvexPolygonShape, Shape);
	PoolVector<Vector3> points;

protected:
	static void _bind_methods();

	virtual void _update_shape();

public:
	void set_points(const PoolVector<Vector3> &p_points);
	PoolVector<Vector3> get_points() const;

	virtual Vector<Vector3> get_debug_mesh_lines();
	virtual real_t get_enclosing_radius() const;

	ConvexPolygonShape();
};

#endif // CONVEX_POLYGON_SHAPE_H
