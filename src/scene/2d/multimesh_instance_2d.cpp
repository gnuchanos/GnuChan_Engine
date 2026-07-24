/**************************************************************************/
/*  multimesh_instance_2d.cpp                                             */
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

#include "multimesh_instance_2d.h"
#include "core/core_string_names.h"

void MultiMeshInstance2D::_notification(int p_what) {
	if (p_what == NOTIFICATION_DRAW) {
		if (multimesh.is_valid()) {
			draw_multimesh(multimesh, texture, normal_map);
		}
	}
}

void MultiMeshInstance2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_multimesh", "multimesh"), &MultiMeshInstance2D::set_multimesh);
	ClassDB::bind_method(D_METHOD("get_multimesh"), &MultiMeshInstance2D::get_multimesh);

	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &MultiMeshInstance2D::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &MultiMeshInstance2D::get_texture);

	ClassDB::bind_method(D_METHOD("set_normal_map", "normal_map"), &MultiMeshInstance2D::set_normal_map);
	ClassDB::bind_method(D_METHOD("get_normal_map"), &MultiMeshInstance2D::get_normal_map);

	ADD_SIGNAL(MethodInfo("texture_changed"));

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "multimesh", PROPERTY_HINT_RESOURCE_TYPE, "MultiMesh"), "set_multimesh", "get_multimesh");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "normal_map", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_normal_map", "get_normal_map");
}

void MultiMeshInstance2D::set_multimesh(const Ref<MultiMesh> &p_multimesh) {
	// Cleanup previous connection if any.
	if (multimesh.is_valid()) {
		multimesh->disconnect(CoreStringNames::get_singleton()->changed, this, "update");
	}
	multimesh = p_multimesh;

	// Connect to the multimesh so the AABB can update when instance transforms are changed.
	if (multimesh.is_valid()) {
		multimesh->connect(CoreStringNames::get_singleton()->changed, this, "update");
	}
	update();
}

Ref<MultiMesh> MultiMeshInstance2D::get_multimesh() const {
	return multimesh;
}

void MultiMeshInstance2D::set_texture(const Ref<Texture> &p_texture) {
	if (p_texture == texture) {
		return;
	}
	texture = p_texture;
	update();
	emit_signal("texture_changed");
	_change_notify("texture");
}

Ref<Texture> MultiMeshInstance2D::get_texture() const {
	return texture;
}

void MultiMeshInstance2D::set_normal_map(const Ref<Texture> &p_texture) {
	normal_map = p_texture;
	update();
}

Ref<Texture> MultiMeshInstance2D::get_normal_map() const {
	return normal_map;
}

#ifdef TOOLS_ENABLED
Rect2 MultiMeshInstance2D::_edit_get_rect() const {
	if (multimesh.is_valid()) {
		AABB aabb = multimesh->get_aabb();
		return Rect2(aabb.position.x, aabb.position.y, aabb.size.x, aabb.size.y);
	}

	return Node2D::_edit_get_rect();
}
#endif

MultiMeshInstance2D::MultiMeshInstance2D() {
}

MultiMeshInstance2D::~MultiMeshInstance2D() {
}
