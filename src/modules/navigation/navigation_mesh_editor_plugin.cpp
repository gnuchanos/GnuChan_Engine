/**************************************************************************/
/*  navigation_mesh_editor_plugin.cpp                                     */
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

#ifdef TOOLS_ENABLED
#include "navigation_mesh_editor_plugin.h"

#include "core/io/marshalls.h"
#include "core/io/resource_saver.h"
#include "navigation_mesh_generator.h"
#include "scene/3d/mesh_instance.h"
#include "scene/gui/box_container.h"

void NavigationMeshEditor::_node_removed(Node *p_node) {
	if (p_node == node) {
		node = nullptr;

		hide();
	}
}

void NavigationMeshEditor::_notification(int p_option) {
	if (p_option == NOTIFICATION_ENTER_TREE) {
		button_bake->set_icon(get_icon("Bake", "EditorIcons"));
		button_reset->set_icon(get_icon("Reload", "EditorIcons"));
	}
}

void NavigationMeshEditor::_bake_pressed() {
	button_bake->set_pressed(false);

	ERR_FAIL_COND(!node);
	if (!node->get_navigation_mesh().is_valid()) {
		err_dialog->set_text(TTR("A NavigationMesh resource must be set or created for this node to work."));
		err_dialog->popup_centered_minsize();
		return;
	}

	NavigationMeshGenerator::get_singleton()->clear(node->get_navigation_mesh());
	NavigationMeshGenerator::get_singleton()->bake(node->get_navigation_mesh(), node);

	node->update_gizmo();
}

void NavigationMeshEditor::_clear_pressed() {
	if (node) {
		NavigationMeshGenerator::get_singleton()->clear(node->get_navigation_mesh());
	}

	button_bake->set_pressed(false);
	bake_info->set_text("");

	if (node) {
		node->update_gizmo();
	}
}

void NavigationMeshEditor::edit(NavigationMeshInstance *p_nav_mesh_instance) {
	if (p_nav_mesh_instance == nullptr || node == p_nav_mesh_instance) {
		return;
	}

	node = p_nav_mesh_instance;
}

void NavigationMeshEditor::_bind_methods() {
	ClassDB::bind_method("_bake_pressed", &NavigationMeshEditor::_bake_pressed);
	ClassDB::bind_method("_clear_pressed", &NavigationMeshEditor::_clear_pressed);
}

NavigationMeshEditor::NavigationMeshEditor() {
	bake_hbox = memnew(HBoxContainer);

	button_bake = memnew(ToolButton);
	bake_hbox->add_child(button_bake);
	button_bake->set_toggle_mode(true);
	button_bake->set_text(TTR("Bake NavMesh"));
	button_bake->connect("pressed", this, "_bake_pressed");

	button_reset = memnew(ToolButton);
	bake_hbox->add_child(button_reset);
	// No button text, we only use a revert icon which is set when entering the tree.
	button_reset->set_tooltip(TTR("Clear the navigation mesh."));
	button_reset->connect("pressed", this, "_clear_pressed");

	bake_info = memnew(Label);
	bake_hbox->add_child(bake_info);

	err_dialog = memnew(AcceptDialog);
	add_child(err_dialog);
	node = nullptr;
}

NavigationMeshEditor::~NavigationMeshEditor() {
}

void NavigationMeshEditorPlugin::edit(Object *p_object) {
	navigation_mesh_editor->edit(Object::cast_to<NavigationMeshInstance>(p_object));
}

bool NavigationMeshEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("NavigationMeshInstance");
}

void NavigationMeshEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		navigation_mesh_editor->show();
		navigation_mesh_editor->bake_hbox->show();
	} else {
		navigation_mesh_editor->hide();
		navigation_mesh_editor->bake_hbox->hide();
		navigation_mesh_editor->edit(nullptr);
	}
}

NavigationMeshEditorPlugin::NavigationMeshEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	navigation_mesh_editor = memnew(NavigationMeshEditor);
	editor->get_viewport()->add_child(navigation_mesh_editor);
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, navigation_mesh_editor->bake_hbox);
	navigation_mesh_editor->hide();
	navigation_mesh_editor->bake_hbox->hide();
}

NavigationMeshEditorPlugin::~NavigationMeshEditorPlugin() {
}

#endif
