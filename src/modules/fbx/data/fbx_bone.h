/**************************************************************************/
/*  fbx_bone.h                                                            */
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

#ifndef FBX_BONE_H
#define FBX_BONE_H

#include "fbx_node.h"
#include "import_state.h"

#include "fbx_parser/FBXDocument.h"

struct PivotTransform;

struct FBXBone : public Reference {
	uint64_t parent_bone_id = 0;
	uint64_t bone_id = 0;

	bool valid_parent = false; // if the parent bone id is set up.
	String bone_name = String(); // bone name

	bool is_root_bone() const {
		return !valid_parent;
	}

	// GnuChan specific data
	int godot_bone_id = -2; // godot internal bone id assigned after import

	// if a bone / armature is the root then FBX skeleton will contain the bone not any other skeleton.
	// this is to support joints by themselves in scenes
	bool valid_armature_id = false;
	uint64_t armature_id = 0;

	/* link node is the parent bone */
	mutable const FBXDocParser::Geometry *geometry = nullptr;
	mutable const FBXDocParser::ModelLimbNode *limb_node = nullptr;

	void set_node(Ref<FBXNode> p_node) {
		node = p_node;
	}

	// Stores the pivot xform for this bone

	Ref<FBXNode> node = nullptr;
	Ref<FBXBone> parent_bone = nullptr;
	Ref<FBXSkeleton> fbx_skeleton = nullptr;
};

struct FBXSkinDeformer {
	FBXSkinDeformer(Ref<FBXBone> p_bone, const FBXDocParser::Cluster *p_cluster) :
			cluster(p_cluster), bone(p_bone) {}
	~FBXSkinDeformer() {}
	const FBXDocParser::Cluster *cluster;
	Ref<FBXBone> bone;

	/* get associate model - the model can be invalid sometimes */
	Ref<FBXBone> get_associate_model() const {
		return bone->parent_bone;
	}

	Ref<FBXNode> get_link(const ImportState &state) const;
};

#endif // FBX_BONE_H
