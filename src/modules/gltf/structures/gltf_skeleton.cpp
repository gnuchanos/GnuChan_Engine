/**************************************************************************/
/*  gltf_skeleton.cpp                                                     */
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

#include "gltf_skeleton.h"

#include "../gltf_template_convert.h"
#include "scene/3d/bone_attachment.h"

void GLTFSkeleton::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_joints"), &GLTFSkeleton::get_joints);
	ClassDB::bind_method(D_METHOD("set_joints", "joints"), &GLTFSkeleton::set_joints);
	ClassDB::bind_method(D_METHOD("get_roots"), &GLTFSkeleton::get_roots);
	ClassDB::bind_method(D_METHOD("set_roots", "roots"), &GLTFSkeleton::set_roots);
	ClassDB::bind_method(D_METHOD("get_godot_skeleton"), &GLTFSkeleton::get_godot_skeleton);
	ClassDB::bind_method(D_METHOD("get_unique_names"), &GLTFSkeleton::get_unique_names);
	ClassDB::bind_method(D_METHOD("set_unique_names", "unique_names"), &GLTFSkeleton::set_unique_names);
	ClassDB::bind_method(D_METHOD("get_godot_bone_node"), &GLTFSkeleton::get_godot_bone_node);
	ClassDB::bind_method(D_METHOD("set_godot_bone_node", "godot_bone_node"), &GLTFSkeleton::set_godot_bone_node);
	ClassDB::bind_method(D_METHOD("get_bone_attachment_count"), &GLTFSkeleton::get_bone_attachment_count);
	ClassDB::bind_method(D_METHOD("get_bone_attachment", "idx"), &GLTFSkeleton::get_bone_attachment);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_INT_ARRAY, "joints"), "set_joints", "get_joints"); // PoolVector<GLTFNodeIndex>
	ADD_PROPERTY(PropertyInfo(Variant::POOL_INT_ARRAY, "roots"), "set_roots", "get_roots"); // PoolVector<GLTFNodeIndex>
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "unique_names", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL | PROPERTY_USAGE_EDITOR), "set_unique_names", "get_unique_names"); // Set<String>
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "godot_bone_node", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL | PROPERTY_USAGE_EDITOR), "set_godot_bone_node", "get_godot_bone_node"); // Map<int32_t,
}

PoolVector<GLTFNodeIndex> GLTFSkeleton::get_joints() {
	return joints;
}

void GLTFSkeleton::set_joints(PoolVector<GLTFNodeIndex> p_joints) {
	joints = p_joints;
}

PoolVector<GLTFNodeIndex> GLTFSkeleton::get_roots() {
	return roots;
}

void GLTFSkeleton::set_roots(PoolVector<GLTFNodeIndex> p_roots) {
	roots = p_roots;
}

Skeleton *GLTFSkeleton::get_godot_skeleton() {
	return godot_skeleton;
}

Array GLTFSkeleton::get_unique_names() {
	return GLTFTemplateConvert::to_array(unique_names);
}

void GLTFSkeleton::set_unique_names(Array p_unique_names) {
	GLTFTemplateConvert::set_from_array(unique_names, p_unique_names);
}

Dictionary GLTFSkeleton::get_godot_bone_node() {
	return GLTFTemplateConvert::to_dict(godot_bone_node);
}

void GLTFSkeleton::set_godot_bone_node(Dictionary p_indict) {
	GLTFTemplateConvert::set_from_dict(godot_bone_node, p_indict);
}

BoneAttachment *GLTFSkeleton::get_bone_attachment(int idx) {
	ERR_FAIL_INDEX_V(idx, bone_attachments.size(), nullptr);
	return bone_attachments[idx];
}

int32_t GLTFSkeleton::get_bone_attachment_count() {
	return bone_attachments.size();
}
