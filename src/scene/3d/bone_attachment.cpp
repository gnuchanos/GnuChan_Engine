/**************************************************************************/
/*  bone_attachment.cpp                                                   */
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

#include "bone_attachment.h"

void BoneAttachment::_validate_property(PropertyInfo &property) const {
	if (property.name == "bone_name") {
		Skeleton *parent = Object::cast_to<Skeleton>(get_parent());

		if (parent) {
			String names;
			for (int i = 0; i < parent->get_bone_count(); i++) {
				if (i > 0) {
					names += ",";
				}
				names += parent->get_bone_name(i);
			}

			property.hint = PROPERTY_HINT_ENUM;
			property.hint_string = names;
		} else {
			property.hint = PROPERTY_HINT_NONE;
			property.hint_string = "";
		}
	}
}

void BoneAttachment::_check_bind() {
	Skeleton *sk = Object::cast_to<Skeleton>(get_parent());
	if (sk) {
		int idx = sk->find_bone(bone_name);
		if (idx != -1) {
			sk->bind_child_node_to_bone(idx, this);
			set_transform(sk->get_bone_global_pose(idx));
			bound = true;
		}
	}
}

void BoneAttachment::_check_unbind() {
	if (bound) {
		Skeleton *sk = Object::cast_to<Skeleton>(get_parent());
		if (sk) {
			int idx = sk->find_bone(bone_name);
			if (idx != -1) {
				sk->unbind_child_node_from_bone(idx, this);
			}
		}
		bound = false;
	}
}

void BoneAttachment::set_bone_name(const String &p_name) {
	if (is_inside_tree()) {
		_check_unbind();
	}

	bone_name = p_name;

	if (is_inside_tree()) {
		_check_bind();
	}
}

String BoneAttachment::get_bone_name() const {
	return bone_name;
}

void BoneAttachment::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_check_bind();
		} break;
		case NOTIFICATION_EXIT_TREE: {
			_check_unbind();
		} break;
	}
}

BoneAttachment::BoneAttachment() {
	bound = false;
}

void BoneAttachment::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_bone_name", "bone_name"), &BoneAttachment::set_bone_name);
	ClassDB::bind_method(D_METHOD("get_bone_name"), &BoneAttachment::get_bone_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "bone_name"), "set_bone_name", "get_bone_name");
}
