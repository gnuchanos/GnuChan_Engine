/**************************************************************************/
/*  gltf_physics_body.h                                                   */
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

#ifndef GLTF_PHYSICS_BODY_H
#define GLTF_PHYSICS_BODY_H

#include "scene/3d/physics_body.h"

// GLTFPhysicsBody is an intermediary between OMI_physics_body and GnuChan's physics body nodes.
// https://github.com/omigroup/gltf-extensions/tree/main/extensions/2.0/OMI_physics_body

class GLTFPhysicsBody : public Resource {
	GDCLASS(GLTFPhysicsBody, Resource)

protected:
	static void _bind_methods();

private:
	String body_type = "static";
	real_t mass = 1.0;
	Vector3 linear_velocity = Vector3();
	Vector3 angular_velocity = Vector3();

public:
	String get_body_type() const;
	void set_body_type(String p_body_type);

	real_t get_mass() const;
	void set_mass(real_t p_mass);

	Vector3 get_linear_velocity() const;
	void set_linear_velocity(Vector3 p_linear_velocity);

	Vector3 get_angular_velocity() const;
	void set_angular_velocity(Vector3 p_angular_velocity);

	static Ref<GLTFPhysicsBody> from_node(const CollisionObject *p_body_node);
	CollisionObject *to_node() const;

	static Ref<GLTFPhysicsBody> from_dictionary(const Dictionary p_dictionary);
	Dictionary to_dictionary() const;
};

#endif // GLTF_PHYSICS_BODY_H
