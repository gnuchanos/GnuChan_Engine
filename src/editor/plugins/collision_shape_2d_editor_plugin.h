/**************************************************************************/
/*  collision_shape_2d_editor_plugin.h                                    */
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

#ifndef COLLISION_SHAPE_2D_EDITOR_PLUGIN_H
#define COLLISION_SHAPE_2D_EDITOR_PLUGIN_H

#include "core/os/input.h"
#include "core/os/keyboard.h"
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"

#include "scene/2d/collision_shape_2d.h"

class CanvasItemEditor;

class CollisionShape2DEditor : public Control {
	GDCLASS(CollisionShape2DEditor, Control);

	enum ShapeType {
		CAPSULE_SHAPE,
		CIRCLE_SHAPE,
		CONCAVE_POLYGON_SHAPE,
		CONVEX_POLYGON_SHAPE,
		LINE_SHAPE,
		RAY_SHAPE,
		RECTANGLE_SHAPE,
		SEGMENT_SHAPE
	};

	const Point2 RECT_HANDLES[8] = {
		Point2(1, 0),
		Point2(1, 1),
		Point2(0, 1),
		Point2(-1, 1),
		Point2(-1, 0),
		Point2(-1, -1),
		Point2(0, -1),
		Point2(1, -1),
	};

	EditorNode *editor;
	UndoRedo *undo_redo;
	CanvasItemEditor *canvas_item_editor;
	CollisionShape2D *node;

	Vector<Point2> handles;

	int shape_type;
	int edit_handle;
	bool pressed;
	real_t grab_threshold = 8;
	Variant original;
	Transform2D original_transform;
	Point2 last_point;

	Variant get_handle_value(int idx) const;
	void set_handle(int idx, Point2 &p_point);
	void commit_handle(int idx, Variant &p_org);

	void _get_current_shape_type();

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	bool forward_canvas_gui_input(const Ref<InputEvent> &p_event);
	void forward_canvas_draw_over_viewport(Control *p_overlay);
	void edit(Node *p_node);

	CollisionShape2DEditor(EditorNode *p_editor);
};

class CollisionShape2DEditorPlugin : public EditorPlugin {
	GDCLASS(CollisionShape2DEditorPlugin, EditorPlugin);

	CollisionShape2DEditor *collision_shape_2d_editor;
	EditorNode *editor;

public:
	virtual bool forward_canvas_gui_input(const Ref<InputEvent> &p_event) { return collision_shape_2d_editor->forward_canvas_gui_input(p_event); }
	virtual void forward_canvas_draw_over_viewport(Control *p_overlay) { collision_shape_2d_editor->forward_canvas_draw_over_viewport(p_overlay); }

	virtual String get_name() const { return "CollisionShape2D"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_obj);
	virtual bool handles(Object *p_obj) const;
	virtual void make_visible(bool visible);

	CollisionShape2DEditorPlugin(EditorNode *p_editor);
	~CollisionShape2DEditorPlugin();
};

#endif // COLLISION_SHAPE_2D_EDITOR_PLUGIN_H
