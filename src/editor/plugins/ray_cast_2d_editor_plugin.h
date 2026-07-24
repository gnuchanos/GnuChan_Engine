/**************************************************************************/
/*  ray_cast_2d_editor_plugin.h                                           */
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

#ifndef RAY_CAST_2D_EDITOR_PLUGIN_H
#define RAY_CAST_2D_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"
#include "scene/2d/ray_cast_2d.h"

class CanvasItemEditor;

class RayCast2DEditor : public Control {
	GDCLASS(RayCast2DEditor, Control);

	UndoRedo *undo_redo = nullptr;
	CanvasItemEditor *canvas_item_editor = nullptr;
	RayCast2D *node = nullptr;

	bool pressed = false;
	Point2 original_cast_to;

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	bool forward_canvas_gui_input(const Ref<InputEvent> &p_event);
	void forward_canvas_draw_over_viewport(Control *p_overlay);
	void edit(Node *p_node);

	RayCast2DEditor();
};

class RayCast2DEditorPlugin : public EditorPlugin {
	GDCLASS(RayCast2DEditorPlugin, EditorPlugin);

	RayCast2DEditor *ray_cast_2d_editor = nullptr;

public:
	virtual bool forward_canvas_gui_input(const Ref<InputEvent> &p_event) { return ray_cast_2d_editor->forward_canvas_gui_input(p_event); }
	virtual void forward_canvas_draw_over_viewport(Control *p_overlay) { ray_cast_2d_editor->forward_canvas_draw_over_viewport(p_overlay); }

	virtual String get_name() const { return "RayCast2D"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool visible);

	RayCast2DEditorPlugin(EditorNode *p_editor);
};

#endif // RAY_CAST_2D_EDITOR_PLUGIN_H
