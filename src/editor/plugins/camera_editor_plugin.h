/**************************************************************************/
/*  camera_editor_plugin.h                                                */
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

#ifndef CAMERA_EDITOR_PLUGIN_H
#define CAMERA_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/camera.h"

class CameraEditor : public Control {
	GDCLASS(CameraEditor, Control);

	Panel *panel;
	Button *preview;
	Node *node;

	void _pressed();

protected:
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	void edit(Node *p_camera);
	CameraEditor();
};

class CameraEditorPlugin : public EditorPlugin {
	GDCLASS(CameraEditorPlugin, EditorPlugin);

	//CameraEditor *camera_editor;
	EditorNode *editor;

public:
	virtual String get_name() const { return "Camera"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	CameraEditorPlugin(EditorNode *p_node);
	~CameraEditorPlugin();
};

#endif // CAMERA_EDITOR_PLUGIN_H
