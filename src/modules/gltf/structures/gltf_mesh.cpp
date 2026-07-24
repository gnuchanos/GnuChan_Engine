/**************************************************************************/
/*  gltf_mesh.cpp                                                         */
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

#include "gltf_mesh.h"

void GLTFMesh::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_mesh"), &GLTFMesh::get_mesh);
	ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &GLTFMesh::set_mesh);
	ClassDB::bind_method(D_METHOD("get_blend_weights"), &GLTFMesh::get_blend_weights);
	ClassDB::bind_method(D_METHOD("set_blend_weights", "blend_weights"), &GLTFMesh::set_blend_weights);
	ClassDB::bind_method(D_METHOD("get_instance_materials"), &GLTFMesh::get_instance_materials);
	ClassDB::bind_method(D_METHOD("set_instance_materials", "instance_materials"), &GLTFMesh::set_instance_materials);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh"), "set_mesh", "get_mesh");
	ADD_PROPERTY(PropertyInfo(Variant::POOL_REAL_ARRAY, "blend_weights"), "set_blend_weights", "get_blend_weights"); // Vector<float>
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "instance_materials"), "set_instance_materials", "get_instance_materials");
}

Ref<ArrayMesh> GLTFMesh::get_mesh() {
	return mesh;
}

void GLTFMesh::set_mesh(Ref<ArrayMesh> p_mesh) {
	mesh = p_mesh;
}

Array GLTFMesh::get_instance_materials() {
	return instance_materials;
}

void GLTFMesh::set_instance_materials(Array p_instance_materials) {
	instance_materials = p_instance_materials;
}

Vector<float> GLTFMesh::get_blend_weights() {
	return blend_weights;
}

void GLTFMesh::set_blend_weights(Vector<float> p_blend_weights) {
	blend_weights = p_blend_weights;
}
