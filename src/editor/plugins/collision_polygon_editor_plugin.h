/**************************************************************************/
/*  collision_polygon_editor_plugin.h                                     */
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

#ifndef COLLISION_POLYGON_EDITOR_PLUGIN_H
#define COLLISION_POLYGON_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/collision_polygon.h"
#include "scene/3d/immediate_geometry.h"
#include "scene/3d/mesh_instance.h"
#include "scene/gui/tool_button.h"

class CanvasItemEditor;

class Polygon3DEditor : public HBoxContainer {
	GDCLASS(Polygon3DEditor, HBoxContainer);

	UndoRedo *undo_redo;
	enum Mode {

		MODE_CREATE,
		MODE_EDIT,

	};

	Mode mode;

	ToolButton *button_create;
	ToolButton *button_edit;

	Ref<Material3D> line_material;
	Ref<Material3D> handle_material;

	EditorNode *editor;
	Panel *panel;
	Spatial *node;
	ImmediateGeometry *imgeom;
	MeshInstance *pointsm;
	Ref<ArrayMesh> m;

	MenuButton *options;

	int edited_point;
	Vector2 edited_point_pos;
	Vector<Vector2> pre_move_edit;
	Vector<Vector2> wip;
	bool wip_active;
	bool snap_ignore;

	float prev_depth;

	void _wip_close();
	void _polygon_draw();
	void _menu_option(int p_option);

	float _get_depth();

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	virtual bool forward_spatial_gui_input(Camera *p_camera, const Ref<InputEvent> &p_event);
	void edit(Node *p_collision_polygon);
	Polygon3DEditor(EditorNode *p_editor);
	~Polygon3DEditor();
};

class Polygon3DEditorPlugin : public EditorPlugin {
	GDCLASS(Polygon3DEditorPlugin, EditorPlugin);

	Polygon3DEditor *collision_polygon_editor;
	EditorNode *editor;

public:
	virtual bool forward_spatial_gui_input(Camera *p_camera, const Ref<InputEvent> &p_event) { return collision_polygon_editor->forward_spatial_gui_input(p_camera, p_event); }

	virtual String get_name() const { return "Polygon3DEditor"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	Polygon3DEditorPlugin(EditorNode *p_node);
	~Polygon3DEditorPlugin();
};

#endif // COLLISION_POLYGON_EDITOR_PLUGIN_H
