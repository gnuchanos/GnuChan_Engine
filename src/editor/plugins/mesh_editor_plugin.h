/**************************************************************************/
/*  mesh_editor_plugin.h                                                  */
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

#ifndef MESH_EDITOR_PLUGIN_H
#define MESH_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/camera.h"
#include "scene/3d/light.h"
#include "scene/3d/mesh_instance.h"
#include "scene/gui/viewport_container.h"
#include "scene/resources/material.h"

class MeshEditor : public ViewportContainer {
	GDCLASS(MeshEditor, ViewportContainer);

	float rot_x;
	float rot_y;

	Viewport *viewport;
	MeshInstance *mesh_instance;
	Spatial *rotation;
	DirectionalLight *light1;
	DirectionalLight *light2;
	Camera *camera;

	Ref<Mesh> mesh;

	TextureButton *light_1_switch;
	TextureButton *light_2_switch;

	void _button_pressed(Node *p_button);
	bool first_enter;

	void _update_rotation();

protected:
	void _notification(int p_what);
	void _gui_input(Ref<InputEvent> p_event);
	static void _bind_methods();

public:
	void edit(Ref<Mesh> p_mesh);
	MeshEditor();
};

class EditorInspectorPluginMesh : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginMesh, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class MeshEditorPlugin : public EditorPlugin {
	GDCLASS(MeshEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const { return "Mesh"; }

	MeshEditorPlugin(EditorNode *p_node);
};

#endif // MESH_EDITOR_PLUGIN_H
