/**************************************************************************/
/*  plane_shape.cpp                                                       */
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

#include "plane_shape.h"

#include "servers/physics_server.h"

Vector<Vector3> PlaneShape::get_debug_mesh_lines() {
	Plane p = get_plane();
	Vector<Vector3> points;

	Vector3 n1 = p.get_any_perpendicular_normal();
	Vector3 n2 = p.normal.cross(n1).normalized();

	Vector3 pface[4] = {
		p.normal * p.d + n1 * 10.0 + n2 * 10.0,
		p.normal * p.d + n1 * 10.0 + n2 * -10.0,
		p.normal * p.d + n1 * -10.0 + n2 * -10.0,
		p.normal * p.d + n1 * -10.0 + n2 * 10.0,
	};

	points.push_back(pface[0]);
	points.push_back(pface[1]);
	points.push_back(pface[1]);
	points.push_back(pface[2]);
	points.push_back(pface[2]);
	points.push_back(pface[3]);
	points.push_back(pface[3]);
	points.push_back(pface[0]);
	points.push_back(p.normal * p.d);
	points.push_back(p.normal * p.d + p.normal * 3);

	return points;
}

void PlaneShape::_update_shape() {
	PhysicsServer::get_singleton()->shape_set_data(get_shape(), plane);
	Shape::_update_shape();
}

void PlaneShape::set_plane(Plane p_plane) {
	plane = p_plane;
	_update_shape();
	notify_change_to_owners();
	_change_notify("plane");
}

Plane PlaneShape::get_plane() const {
	return plane;
}

void PlaneShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_plane", "plane"), &PlaneShape::set_plane);
	ClassDB::bind_method(D_METHOD("get_plane"), &PlaneShape::get_plane);

	ADD_PROPERTY(PropertyInfo(Variant::PLANE, "plane"), "set_plane", "get_plane");
}

PlaneShape::PlaneShape() :
		Shape(RID_PRIME(PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_PLANE))) {
	set_plane(Plane(0, 1, 0, 0));
}
