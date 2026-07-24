/**************************************************************************/
/*  skeleton_ik_editor_plugin.cpp                                         */
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

#include "skeleton_ik_editor_plugin.h"

#include "scene/animation/skeleton_ik.h"

void SkeletonIKEditorPlugin::_play() {
	if (!skeleton_ik) {
		return;
	}

	if (!skeleton_ik->get_parent_skeleton()) {
		return;
	}

	if (play_btn->is_pressed()) {
		skeleton_ik->start();
	} else {
		skeleton_ik->stop();
		skeleton_ik->get_parent_skeleton()->clear_bones_global_pose_override();
	}
}

void SkeletonIKEditorPlugin::edit(Object *p_object) {
	if (p_object != skeleton_ik) {
		if (skeleton_ik) {
			play_btn->set_pressed(false);
			_play();
		}
	}

	SkeletonIK *s = Object::cast_to<SkeletonIK>(p_object);
	if (!s) {
		return;
	}

	skeleton_ik = s;
}

bool SkeletonIKEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("SkeletonIK");
}

void SkeletonIKEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		play_btn->show();
	} else {
		play_btn->hide();
	}
}

void SkeletonIKEditorPlugin::_bind_methods() {
	ClassDB::bind_method("_play", &SkeletonIKEditorPlugin::_play);
}

SkeletonIKEditorPlugin::SkeletonIKEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	play_btn = memnew(Button);
	play_btn->set_icon(editor->get_gui_base()->get_icon("Play", "EditorIcons"));
	play_btn->set_text(TTR("Play IK"));
	play_btn->set_toggle_mode(true);
	play_btn->hide();
	play_btn->connect("pressed", this, "_play");
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, play_btn);
	skeleton_ik = nullptr;
}

SkeletonIKEditorPlugin::~SkeletonIKEditorPlugin() {}
