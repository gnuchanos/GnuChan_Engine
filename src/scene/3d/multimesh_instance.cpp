/**************************************************************************/
/*  multimesh_instance.cpp                                                */
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

#include "multimesh_instance.h"

void MultiMeshInstance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_multimesh", "multimesh"), &MultiMeshInstance::set_multimesh);
	ClassDB::bind_method(D_METHOD("get_multimesh"), &MultiMeshInstance::get_multimesh);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "multimesh", PROPERTY_HINT_RESOURCE_TYPE, "MultiMesh"), "set_multimesh", "get_multimesh");
}

void MultiMeshInstance::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		_refresh_interpolated();
	}
}

void MultiMeshInstance::set_multimesh(const Ref<MultiMesh> &p_multimesh) {
	multimesh = p_multimesh;
	if (multimesh.is_valid()) {
		set_base(multimesh->get_rid());
		_refresh_interpolated();
	} else {
		set_base(RID());
	}
}

void MultiMeshInstance::_refresh_interpolated() {
	if (is_inside_tree() && multimesh.is_valid()) {
		bool interpolated = is_physics_interpolated_and_enabled();
		multimesh->set_physics_interpolated(interpolated);
	}
}

Ref<MultiMesh> MultiMeshInstance::get_multimesh() const {
	return multimesh;
}

PoolVector<Face3> MultiMeshInstance::get_faces(uint32_t p_usage_flags) const {
	return PoolVector<Face3>();
}

AABB MultiMeshInstance::get_aabb() const {
	if (multimesh.is_null()) {
		return AABB();
	} else {
		return multimesh->get_aabb();
	}
}

void MultiMeshInstance::_physics_interpolated_changed() {
	VisualInstance::_physics_interpolated_changed();
	_refresh_interpolated();
}

MultiMeshInstance::MultiMeshInstance() {
}

MultiMeshInstance::~MultiMeshInstance() {
}
