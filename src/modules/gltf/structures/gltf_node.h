/**************************************************************************/
/*  gltf_node.h                                                           */
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

#ifndef GLTF_NODE_H
#define GLTF_NODE_H

#include "../gltf_defines.h"
#include "core/resource.h"

class GLTFNode : public Resource {
	GDCLASS(GLTFNode, Resource);
	friend class GLTFDocument;
	friend class PackedSceneGLTF;

private:
	// matrices need to be transformed to this
	GLTFNodeIndex parent = -1;
	int height = -1;
	Transform xform;
	GLTFMeshIndex mesh = -1;
	GLTFCameraIndex camera = -1;
	GLTFSkinIndex skin = -1;
	GLTFSkeletonIndex skeleton = -1;
	bool joint = false;
	Vector3 translation;
	Quat rotation;
	Vector3 scale = Vector3(1, 1, 1);
	Vector<int> children;
	GLTFLightIndex light = -1;
	Dictionary additional_data;

protected:
	static void _bind_methods();

public:
	GLTFNodeIndex get_parent();
	void set_parent(GLTFNodeIndex p_parent);

	int get_height();
	void set_height(int p_height);

	Transform get_xform();
	void set_xform(Transform p_xform);

	GLTFMeshIndex get_mesh();
	void set_mesh(GLTFMeshIndex p_mesh);

	GLTFCameraIndex get_camera();
	void set_camera(GLTFCameraIndex p_camera);

	GLTFSkinIndex get_skin();
	void set_skin(GLTFSkinIndex p_skin);

	GLTFSkeletonIndex get_skeleton();
	void set_skeleton(GLTFSkeletonIndex p_skeleton);

	bool get_joint();
	void set_joint(bool p_joint);

	Vector3 get_translation();
	void set_translation(Vector3 p_translation);

	Quat get_rotation();
	void set_rotation(Quat p_rotation);

	Vector3 get_scale();
	void set_scale(Vector3 p_scale);

	Vector<int> get_children();
	void set_children(Vector<int> p_children);

	GLTFLightIndex get_light();
	void set_light(GLTFLightIndex p_light);

	Variant get_additional_data(const String &p_extension_name);
	void set_additional_data(const String &p_extension_name, Variant p_additional_data);
};

#endif // GLTF_NODE_H
