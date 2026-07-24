/**************************************************************************/
/*  gltf_skeleton.h                                                       */
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

#ifndef GLTF_SKELETON_H
#define GLTF_SKELETON_H

#include "../gltf_defines.h"
#include "core/resource.h"

class GLTFSkeleton : public Resource {
	GDCLASS(GLTFSkeleton, Resource);
	friend class GLTFDocument;

private:
	// The *synthesized* skeletons joints
	PoolVector<GLTFNodeIndex> joints;

	// The roots of the skeleton. If there are multiple, each root must have the
	// same parent (ie roots are siblings)
	PoolVector<GLTFNodeIndex> roots;

	// The created Skeleton for the scene
	Skeleton *godot_skeleton = nullptr;

	// Set of unique bone names for the skeleton
	Set<String> unique_names;

	Map<int32_t, GLTFNodeIndex> godot_bone_node;

	PoolVector<BoneAttachment *> bone_attachments;

protected:
	static void _bind_methods();

public:
	PoolVector<GLTFNodeIndex> get_joints();
	void set_joints(PoolVector<GLTFNodeIndex> p_joints);

	PoolVector<GLTFNodeIndex> get_roots();
	void set_roots(PoolVector<GLTFNodeIndex> p_roots);

	Skeleton *get_godot_skeleton();

	Array get_unique_names();
	void set_unique_names(Array p_unique_names);

	Dictionary get_godot_bone_node();
	void set_godot_bone_node(Dictionary p_indict);

	BoneAttachment *get_bone_attachment(int idx);

	int32_t get_bone_attachment_count();
};

#endif // GLTF_SKELETON_H
