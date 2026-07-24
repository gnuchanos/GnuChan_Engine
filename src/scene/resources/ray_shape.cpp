/**************************************************************************/
/*  ray_shape.cpp                                                         */
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

#include "ray_shape.h"

#include "servers/physics_server.h"

Vector<Vector3> RayShape::get_debug_mesh_lines() {
	Vector<Vector3> points;
	points.push_back(Vector3());
	points.push_back(Vector3(0, 0, get_length()));

	return points;
}

real_t RayShape::get_enclosing_radius() const {
	return length;
}

void RayShape::_update_shape() {
	Dictionary d;
	d["length"] = length;
	d["slips_on_slope"] = slips_on_slope;
	PhysicsServer::get_singleton()->shape_set_data(get_shape(), d);
	Shape::_update_shape();
}

void RayShape::set_length(float p_length) {
	length = p_length;
	_update_shape();
	notify_change_to_owners();
	_change_notify("length");
}

float RayShape::get_length() const {
	return length;
}

void RayShape::set_slips_on_slope(bool p_active) {
	slips_on_slope = p_active;
	_update_shape();
	notify_change_to_owners();
	_change_notify("slips_on_slope");
}

bool RayShape::get_slips_on_slope() const {
	return slips_on_slope;
}

void RayShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_length", "length"), &RayShape::set_length);
	ClassDB::bind_method(D_METHOD("get_length"), &RayShape::get_length);

	ClassDB::bind_method(D_METHOD("set_slips_on_slope", "active"), &RayShape::set_slips_on_slope);
	ClassDB::bind_method(D_METHOD("get_slips_on_slope"), &RayShape::get_slips_on_slope);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "length", PROPERTY_HINT_RANGE, "0.001,100,0.001,or_greater"), "set_length", "get_length");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "slips_on_slope"), "set_slips_on_slope", "get_slips_on_slope");
}

RayShape::RayShape() :
		Shape(RID_PRIME(PhysicsServer::get_singleton()->shape_create(PhysicsServer::SHAPE_RAY))) {
	length = 1.0;
	slips_on_slope = false;

	/* Code copied from setters to prevent the use of uninitialized variables */
	_update_shape();
	notify_change_to_owners();
	_change_notify("length");
	_change_notify("slips_on_slope");
}
