/**************************************************************************/
/*  capsule_shape.cpp                                                     */
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

#include "capsule_shape.h"

#include "servers/physics_server.h"

Vector<Vector3> CapsuleShape::get_debug_mesh_lines() {
	float radius = get_radius();
	float height = get_height();

	Vector<Vector3> points;

	Vector3 d(0, 0, height * 0.5);
	for (int i = 0; i < 360; i++) {
		float ra = Math::deg2rad((float)i);
		float rb = Math::deg2rad((float)i + 1);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;

		points.push_back(Vector3(a.x, a.y, 0) + d);
		points.push_back(Vector3(b.x, b.y, 0) + d);

		points.push_back(Vector3(a.x, a.y, 0) - d);
		points.push_back(Vector3(b.x, b.y, 0) - d);

		if (i % 90 == 0) {
			points.push_back(Vector3(a.x, a.y, 0) + d);
			points.push_back(Vector3(a.x, a.y, 0) - d);
		}

		Vector3 dud = i < 180 ? d : -d;

		points.push_back(Vector3(0, a.y, a.x) + dud);
		points.push_back(Vector3(0, b.y, b.x) + dud);
		points.push_back(Vector3(a.y, 0, a.x) + dud);
		points.push_back(Vector3(b.y, 0, b.x) + dud);
	}

	return points;
}

real_t CapsuleShape::get_enclosing_radius() const {
	return radius + height * 0.5;
}

void CapsuleShape::_update_shape() {
	Dictionary d;
	d["radius"] = radius;
	d["height"] = height;
	PhysicsServer::get_singleton()->shape_set_data(get_shape(), d);
	Shape::_update_shape();
}

void CapsuleShape::set_radius(float p_radius) {
	radius = p_radius;
	_update_shape();
	notify_change_to_owners();
	_change_notify("radius");
}

float CapsuleShape::get_radius() const {
	return radius;
}

void CapsuleShape::set_height(float p_height) {
	height = p_height;
	_update_shape();
	notify_change_to_owners();
	_change_notify("height");
}

float CapsuleShape::get_height() const {
	return height;
}

void CapsuleShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &CapsuleShape::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &CapsuleShape::get_radius);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &CapsuleShape::set_height);
	ClassDB::bind_method(D_METHOD("get_height"), &CapsuleShape::get_height);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.001,100,0.001,or_greater"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "height", PROPERTY_HINT_RANGE, "0.001,100,0.001,or_greater"), "set_height", "get_height");
}

CapsuleShape::CapsuleShape() :
		Shape(RID_PRIME(PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_CAPSULE))) {
	radius = 1.0;
	height = 1.0;
	_update_shape();
}
