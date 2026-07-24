/**************************************************************************/
/*  box_shape.cpp                                                         */
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

#include "box_shape.h"
#include "servers/physics_server.h"

Vector<Vector3> BoxShape::get_debug_mesh_lines() {
	Vector<Vector3> lines;
	AABB aabb;
	aabb.position = -get_extents();
	aabb.size = aabb.position * -2;

	for (int i = 0; i < 12; i++) {
		Vector3 a, b;
		aabb.get_edge(i, a, b);
		lines.push_back(a);
		lines.push_back(b);
	}

	return lines;
}

real_t BoxShape::get_enclosing_radius() const {
	return extents.length();
}

void BoxShape::_update_shape() {
	PhysicsServer::get_singleton()->shape_set_data(get_shape(), extents);
	Shape::_update_shape();
}

void BoxShape::set_extents(const Vector3 &p_extents) {
	extents = p_extents;
	_update_shape();
	notify_change_to_owners();
	_change_notify("extents");
}

Vector3 BoxShape::get_extents() const {
	return extents;
}

void BoxShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_extents", "extents"), &BoxShape::set_extents);
	ClassDB::bind_method(D_METHOD("get_extents"), &BoxShape::get_extents);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "extents"), "set_extents", "get_extents");
}

BoxShape::BoxShape() :
		Shape(RID_PRIME(PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_BOX))) {
	set_extents(Vector3(1, 1, 1));
}
