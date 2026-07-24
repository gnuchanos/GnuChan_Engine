/**************************************************************************/
/*  gltf_collider.h                                                       */
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

#ifndef GLTF_COLLIDER_H
#define GLTF_COLLIDER_H

#include "../../gltf_defines.h"
#include "scene/3d/collision_shape.h"

class GLTFState;

// GLTFCollider is an intermediary between OMI_collider and GnuChan's collision shape nodes.
// https://github.com/omigroup/gltf-extensions/tree/main/extensions/2.0/OMI_collider

class GLTFCollider : public Resource {
	GDCLASS(GLTFCollider, Resource)

protected:
	static void _bind_methods();

private:
	String shape_type;
	Vector3 size = Vector3(1.0, 1.0, 1.0);
	real_t radius = 0.5;
	real_t height = 2.0;
	bool is_trigger = false;
	GLTFMeshIndex mesh_index = -1;
	Ref<ArrayMesh> array_mesh = nullptr;
	// Internal only, for caching GnuChan shape resources. Used in `to_node`.
	Ref<Shape> _shape_cache = nullptr;

public:
	String get_shape_type() const;
	void set_shape_type(String p_shape_type);

	Vector3 get_size() const;
	void set_size(Vector3 p_size);

	real_t get_radius() const;
	void set_radius(real_t p_radius);

	real_t get_height() const;
	void set_height(real_t p_height);

	bool get_is_trigger() const;
	void set_is_trigger(bool p_is_trigger);

	GLTFMeshIndex get_mesh_index() const;
	void set_mesh_index(GLTFMeshIndex p_mesh_index);

	Ref<ArrayMesh> get_array_mesh() const;
	void set_array_mesh(Ref<ArrayMesh> p_array_mesh);

	static Ref<GLTFCollider> from_node(const CollisionShape *p_collider_node);
	CollisionShape *to_node(bool p_cache_shapes = false);

	static Ref<GLTFCollider> from_dictionary(const Dictionary p_dictionary);
	Dictionary to_dictionary() const;
};

#endif // GLTF_COLLIDER_H
