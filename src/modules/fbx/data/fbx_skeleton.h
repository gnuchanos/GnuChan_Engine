/**************************************************************************/
/*  fbx_skeleton.h                                                        */
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

#ifndef FBX_SKELETON_H
#define FBX_SKELETON_H

#include "fbx_bone.h"
#include "fbx_node.h"
#include "model_abstraction.h"

#include "core/reference.h"
#include "scene/3d/skeleton.h"

struct FBXNode;
struct ImportState;
struct FBXBone;

struct FBXSkeleton : Reference {
	Ref<FBXNode> fbx_node = Ref<FBXNode>();
	Vector<Ref<FBXBone>> skeleton_bones = Vector<Ref<FBXBone>>();
	Skeleton *skeleton = nullptr;

	void init_skeleton(const ImportState &state);
};

#endif // FBX_SKELETON_H
