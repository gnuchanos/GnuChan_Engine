/**************************************************************************/
/*  skeleton_2d_editor_plugin.h                                           */
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

#ifndef SKELETON_2D_EDITOR_PLUGIN_H
#define SKELETON_2D_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/2d/skeleton_2d.h"
#include "scene/gui/spin_box.h"

class Skeleton2DEditor : public Control {
	GDCLASS(Skeleton2DEditor, Control);

	enum Menu {
		MENU_OPTION_SET_REST,
		MENU_OPTION_MAKE_REST,
	};

	Skeleton2D *node;

	MenuButton *options;
	AcceptDialog *err_dialog;

	void _menu_option(int p_option);

	//void _create_uv_lines();
	friend class Skeleton2DEditorPlugin;

protected:
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	void edit(Skeleton2D *p_sprite);
	Skeleton2DEditor();
};

class Skeleton2DEditorPlugin : public EditorPlugin {
	GDCLASS(Skeleton2DEditorPlugin, EditorPlugin);

	Skeleton2DEditor *sprite_editor;
	EditorNode *editor;

public:
	virtual String get_name() const { return "Skeleton2D"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	Skeleton2DEditorPlugin(EditorNode *p_node);
	~Skeleton2DEditorPlugin();
};

#endif // SKELETON_2D_EDITOR_PLUGIN_H
