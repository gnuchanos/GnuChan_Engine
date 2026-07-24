/**************************************************************************/
/*  sphere_shape.cpp                                                      */
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

#include "sphere_shape.h"

#include "servers/physics_server.h"

Vector<Vector3> SphereShape::get_debug_mesh_lines() {
	float r = get_radius();

	Vector<Vector3> points;

	for (int i = 0; i <= 360; i++) {
		float ra = Math::deg2rad((float)i);
		float rb = Math::deg2rad((float)i + 1);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

		points.push_back(Vector3(a.x, 0, a.y));
		points.push_back(Vector3(b.x, 0, b.y));
		points.push_back(Vector3(0, a.x, a.y));
		points.push_back(Vector3(0, b.x, b.y));
		points.push_back(Vector3(a.x, a.y, 0));
		points.push_back(Vector3(b.x, b.y, 0));
	}

	return points;
}

real_t SphereShape::get_enclosing_radius() const {
	return radius;
}

void SphereShape::_update_shape() {
	PhysicsServer::get_singleton()->shape_set_data(get_shape(), radius);
	Shape::_update_shape();
}

void SphereShape::set_radius(float p_radius) {
	radius = p_radius;
	_update_shape();
	notify_change_to_owners();
	_change_notify("radius");
}

float SphereShape::get_radius() const {
	return radius;
}

void SphereShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &SphereShape::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &SphereShape::get_radius);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.001,100,0.001,or_greater"), "set_radius", "get_radius");
}

SphereShape::SphereShape() :
		Shape(RID_PRIME(PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_SPHERE))) {
	set_radius(1.0);
}
