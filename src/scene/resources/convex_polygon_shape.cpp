/**************************************************************************/
/*  convex_polygon_shape.cpp                                              */
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

#include "convex_polygon_shape.h"
#include "core/math/convex_hull.h"
#include "servers/physics_server.h"

Vector<Vector3> ConvexPolygonShape::get_debug_mesh_lines() {
	PoolVector<Vector3> points = get_points();

	if (points.size() > 3) {
		Vector<Vector3> varr = Variant(points);
		Geometry::MeshData md;
		Error err = ConvexHullComputer::convex_hull(varr, md);
		if (err == OK) {
			Vector<Vector3> lines;
			lines.resize(md.edges.size() * 2);
			for (int i = 0; i < md.edges.size(); i++) {
				lines.write[i * 2 + 0] = md.vertices[md.edges[i].a];
				lines.write[i * 2 + 1] = md.vertices[md.edges[i].b];
			}
			return lines;
		}
	}

	return Vector<Vector3>();
}

real_t ConvexPolygonShape::get_enclosing_radius() const {
	PoolVector<Vector3> data = get_points();
	PoolVector<Vector3>::Read read = data.read();
	real_t r = 0;
	for (int i(0); i < data.size(); i++) {
		r = MAX(read[i].length_squared(), r);
	}
	return Math::sqrt(r);
}

void ConvexPolygonShape::_update_shape() {
	PhysicsServer::get_singleton()->shape_set_data(get_shape(), points);
	Shape::_update_shape();
}

void ConvexPolygonShape::set_points(const PoolVector<Vector3> &p_points) {
	points = p_points;
	_update_shape();
	notify_change_to_owners();
}

PoolVector<Vector3> ConvexPolygonShape::get_points() const {
	return points;
}

void ConvexPolygonShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_points", "points"), &ConvexPolygonShape::set_points);
	ClassDB::bind_method(D_METHOD("get_points"), &ConvexPolygonShape::get_points);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "points"), "set_points", "get_points");
}

ConvexPolygonShape::ConvexPolygonShape() :
		Shape(RID_PRIME(PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_CONVEX_POLYGON))) {
}
