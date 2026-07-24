/**************************************************************************/
/*  fbx_node.h                                                            */
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

#ifndef FBX_NODE_H
#define FBX_NODE_H

#include "fbx_skeleton.h"
#include "model_abstraction.h"
#include "pivot_transform.h"

#include "fbx_parser/FBXDocument.h"

class Spatial;
struct PivotTransform;

struct FBXNode : Reference, ModelAbstraction {
	uint64_t current_node_id = 0;
	String node_name = String();
	Spatial *godot_node = nullptr;

	// used to parent the skeleton once the tree is built.
	Ref<FBXSkeleton> skeleton_node = Ref<FBXSkeleton>();

	void set_parent(Ref<FBXNode> p_parent) {
		fbx_parent = p_parent;
	}

	void set_pivot_transform(Ref<PivotTransform> p_pivot_transform) {
		pivot_transform = p_pivot_transform;
	}

	Ref<PivotTransform> pivot_transform = Ref<PivotTransform>(); // local and global xform data
	Ref<FBXNode> fbx_parent = Ref<FBXNode>(); // parent node
};

#endif // FBX_NODE_H
