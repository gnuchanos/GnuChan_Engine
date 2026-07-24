/**************************************************************************/
/*  editor_scene_exporter_gltf_plugin.h                                   */
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

#ifndef EDITOR_SCENE_EXPORTER_GLTF_PLUGIN_H
#define EDITOR_SCENE_EXPORTER_GLTF_PLUGIN_H

#include "editor/editor_file_dialog.h"
#include "editor/editor_plugin.h"

#include "packed_scene_gltf.h"

class SceneExporterGLTFPlugin : public EditorPlugin {
	GDCLASS(SceneExporterGLTFPlugin, EditorPlugin);

	Ref<PackedSceneGLTF> convert_gltf2;
	EditorNode *editor = nullptr;
	EditorFileDialog *file_export_lib = nullptr;
	void _gltf2_dialog_action(String p_file);
	void convert_scene_to_gltf2(Variant p_null);

protected:
	static void _bind_methods();

public:
	virtual String get_name() const;
	bool has_main_screen() const;
	SceneExporterGLTFPlugin(class EditorNode *p_node);
};

#endif // EDITOR_SCENE_EXPORTER_GLTF_PLUGIN_H
