/**************************************************************************/
/*  gltf_mesh.h                                                           */
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

#ifndef GLTF_MESH_H
#define GLTF_MESH_H

#include "core/resource.h"
#include "scene/resources/mesh.h"

class GLTFMesh : public Resource {
	GDCLASS(GLTFMesh, Resource);

private:
	Ref<ArrayMesh> mesh;
	Vector<float> blend_weights;
	Array instance_materials;

protected:
	static void _bind_methods();

public:
	Ref<ArrayMesh> get_mesh();
	void set_mesh(Ref<ArrayMesh> p_mesh);
	Vector<float> get_blend_weights();
	void set_blend_weights(Vector<float> p_blend_weights);
	Array get_instance_materials();
	void set_instance_materials(Array p_instance_materials);
};

#endif // GLTF_MESH_H
