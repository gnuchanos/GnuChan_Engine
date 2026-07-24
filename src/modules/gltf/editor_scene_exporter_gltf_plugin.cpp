/**************************************************************************/
/*  editor_scene_exporter_gltf_plugin.cpp                                 */
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

#include "editor_scene_exporter_gltf_plugin.h"

#include "editor/editor_file_dialog.h"
#include "editor/editor_file_system.h"
#include "editor/editor_node.h"
#include "scene/main/node.h"

String SceneExporterGLTFPlugin::get_name() const {
	return "ConvertGLTF2";
}

bool SceneExporterGLTFPlugin::has_main_screen() const {
	return false;
}

SceneExporterGLTFPlugin::SceneExporterGLTFPlugin(EditorNode *p_node) {
	editor = p_node;
	convert_gltf2.instance();
	file_export_lib = memnew(EditorFileDialog);
	editor->get_gui_base()->add_child(file_export_lib);
	file_export_lib->connect("file_selected", this, "_gltf2_dialog_action");
	file_export_lib->set_title(TTR("Export Library"));
	file_export_lib->set_mode(EditorFileDialog::MODE_SAVE_FILE);
	file_export_lib->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
	file_export_lib->clear_filters();
	file_export_lib->add_filter("*.glb");
	file_export_lib->add_filter("*.gltf");
	file_export_lib->set_title(TTR("Export Mesh GLTF2"));
	String gltf_scene_name = TTR("Export GLTF...");
	add_tool_menu_item(gltf_scene_name, this, "convert_scene_to_gltf2", DEFVAL(Variant()));
}

void SceneExporterGLTFPlugin::_gltf2_dialog_action(String p_file) {
	Node *root = editor->get_tree()->get_edited_scene_root();
	if (!root) {
		editor->show_accept(TTR("This operation can't be done without a scene."), TTR("OK"));
		return;
	}
	List<String> deps;
	convert_gltf2->save_scene(root, p_file, p_file, 0, 1000.0f, &deps);
	EditorFileSystem::get_singleton()->scan_changes();
}

void SceneExporterGLTFPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("convert_scene_to_gltf2"), &SceneExporterGLTFPlugin::convert_scene_to_gltf2);
	ClassDB::bind_method(D_METHOD("_gltf2_dialog_action", "file"), &SceneExporterGLTFPlugin::_gltf2_dialog_action);
}

void SceneExporterGLTFPlugin::convert_scene_to_gltf2(Variant p_null) {
	Node *root = editor->get_tree()->get_edited_scene_root();
	if (!root) {
		editor->show_accept(TTR("This operation can't be done without a scene."), TTR("OK"));
		return;
	}
	String filename = String(root->get_filename().get_file().get_basename());
	if (filename.empty()) {
		filename = root->get_name();
	}
	file_export_lib->set_current_file(filename + ".gltf");
	file_export_lib->popup_centered_ratio();
}

#endif // TOOLS_ENABLED
