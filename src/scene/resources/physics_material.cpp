/**************************************************************************/
/*  physics_material.cpp                                                  */
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

#include "physics_material.h"

void PhysicsMaterial::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_friction", "friction"), &PhysicsMaterial::set_friction);
	ClassDB::bind_method(D_METHOD("get_friction"), &PhysicsMaterial::get_friction);

	ClassDB::bind_method(D_METHOD("set_rough", "rough"), &PhysicsMaterial::set_rough);
	ClassDB::bind_method(D_METHOD("is_rough"), &PhysicsMaterial::is_rough);

	ClassDB::bind_method(D_METHOD("set_bounce", "bounce"), &PhysicsMaterial::set_bounce);
	ClassDB::bind_method(D_METHOD("get_bounce"), &PhysicsMaterial::get_bounce);

	ClassDB::bind_method(D_METHOD("set_absorbent", "absorbent"), &PhysicsMaterial::set_absorbent);
	ClassDB::bind_method(D_METHOD("is_absorbent"), &PhysicsMaterial::is_absorbent);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "friction", PROPERTY_HINT_RANGE, "0,1,0.01,or_greater"), "set_friction", "get_friction");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rough"), "set_rough", "is_rough");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "bounce", PROPERTY_HINT_RANGE, "0,1,0.01,or_greater"), "set_bounce", "get_bounce");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "absorbent"), "set_absorbent", "is_absorbent");
}

void PhysicsMaterial::set_friction(real_t p_val) {
	friction = p_val;
	emit_changed();
}

void PhysicsMaterial::set_rough(bool p_val) {
	rough = p_val;
	emit_changed();
}

void PhysicsMaterial::set_bounce(real_t p_val) {
	bounce = p_val;
	emit_changed();
}

void PhysicsMaterial::set_absorbent(bool p_val) {
	absorbent = p_val;
	emit_changed();
}

PhysicsMaterial::PhysicsMaterial() :
		friction(1),
		rough(false),
		bounce(0),
		absorbent(false) {}
