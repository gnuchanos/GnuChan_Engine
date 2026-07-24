/**************************************************************************/
/*  fbx_bone.cpp                                                          */
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

#include "fbx_bone.h"

#include "fbx_node.h"
#include "import_state.h"

Ref<FBXNode> FBXSkinDeformer::get_link(const ImportState &state) const {
	print_verbose("bone name: " + bone->bone_name);

	// safe for when deformers must be polyfilled when skin has different count of binds to bones in the scene ;)
	if (!cluster) {
		return nullptr;
	}

	ERR_FAIL_COND_V_MSG(cluster->TargetNode() == nullptr, nullptr, "bone has invalid target node");

	Ref<FBXNode> link_node;
	uint64_t id = cluster->TargetNode()->ID();
	if (state.fbx_target_map.has(id)) {
		link_node = state.fbx_target_map[id];
	} else {
		print_error("link node not found for " + itos(id));
	}

	// the node in space this is for, like if it's FOR a target.
	return link_node;
}
