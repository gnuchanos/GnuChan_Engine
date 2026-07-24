/**************************************************************************/
/*  packed_scene_editor_plugin.h                                          */
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

#ifndef PACKED_SCENE_EDITOR_PLUGIN_H
#define PACKED_SCENE_EDITOR_PLUGIN_H

#include "editor/editor_inspector.h"
#include "editor/editor_plugin.h"
#include "scene/gui/box_container.h"

class PackedSceneEditor : public VBoxContainer {
	GDCLASS(PackedSceneEditor, VBoxContainer);

	Ref<PackedScene> packed_scene;
	Button *open_scene_button;

	void _on_open_scene_pressed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	PackedSceneEditor(Ref<PackedScene> &p_packed_scene);
};

class EditorInspectorPluginPackedScene : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginPackedScene, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class PackedSceneEditorPlugin : public EditorPlugin {
	GDCLASS(PackedSceneEditorPlugin, EditorPlugin);

public:
	PackedSceneEditorPlugin(EditorNode *p_editor);
};

#endif // PACKED_SCENE_EDITOR_PLUGIN_H
