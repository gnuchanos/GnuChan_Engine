/**************************************************************************/
/*  gradient_texture_2d_editor_plugin.h                                   */
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

#ifndef GRADIENT_TEXTURE_2D_EDITOR_PLUGIN_H
#define GRADIENT_TEXTURE_2D_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"
#include "editor/editor_spin_slider.h"

class GradientTexture2DEditorRect : public Control {
	GDCLASS(GradientTexture2DEditorRect, Control);

	enum Handle {
		HANDLE_NONE,
		HANDLE_FILL_FROM,
		HANDLE_FILL_TO
	};

	Ref<GradientTexture2D> texture;
	UndoRedo *undo_redo = nullptr;
	bool snap_enabled = false;
	float snap_size = 0;

	TextureRect *checkerboard = nullptr;

	Handle handle = HANDLE_NONE;
	Size2 handle_size;
	Point2 offset;
	Size2 size;

	Point2 _get_handle_position(const Handle p_handle);
	void _update_fill_position();
	void _gui_input(const Ref<InputEvent> &p_event);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_texture(Ref<GradientTexture2D> &p_texture);
	void set_snap_enabled(bool p_snap_enabled);
	void set_snap_size(float p_snap_size);

	GradientTexture2DEditorRect();
};

class GradientTexture2DEditor : public VBoxContainer {
	GDCLASS(GradientTexture2DEditor, VBoxContainer);

	Ref<GradientTexture2D> texture;
	UndoRedo *undo_redo = nullptr;

	Button *reverse_button = nullptr;
	Button *snap_button = nullptr;
	EditorSpinSlider *snap_size_edit = nullptr;
	GradientTexture2DEditorRect *texture_editor_rect = nullptr;

	void _reverse_button_pressed();
	void _set_snap_enabled(bool p_enabled);
	void _set_snap_size(float p_snap_size);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_texture(Ref<GradientTexture2D> &p_texture);

	GradientTexture2DEditor();
};

class EditorInspectorPluginGradientTexture2D : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginGradientTexture2D, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class GradientTexture2DEditorPlugin : public EditorPlugin {
	GDCLASS(GradientTexture2DEditorPlugin, EditorPlugin);

public:
	GradientTexture2DEditorPlugin(EditorNode *p_node);
};

#endif // GRADIENT_TEXTURE_2D_EDITOR_PLUGIN_H
