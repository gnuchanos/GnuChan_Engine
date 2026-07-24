/**************************************************************************/
/*  editor_sub_scene.h                                                    */
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

#ifndef EDITOR_SUB_SCENE_H
#define EDITOR_SUB_SCENE_H

#include "editor/editor_file_dialog.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"

class EditorSubScene : public ConfirmationDialog {
	GDCLASS(EditorSubScene, ConfirmationDialog);

	List<Node *> selection;
	LineEdit *path;
	Tree *tree;
	Node *scene;
	bool is_root;

	EditorFileDialog *file_dialog;

	void _fill_tree(Node *p_node, TreeItem *p_parent);
	void _selected_changed();
	void _item_multi_selected(Object *p_object, int p_cell, bool p_selected);
	void _remove_selection_child(Node *p_node);
	void _reown(Node *p_node, List<Node *> *p_to_reown);

	void ok_pressed();

protected:
	void _notification(int p_what);
	static void _bind_methods();
	void _path_browse();
	void _path_selected(const String &p_path);
	void _path_changed(const String &p_path);

public:
	void move(Node *p_new_parent, Node *p_new_owner);
	void clear();
	EditorSubScene();
};

#endif // EDITOR_SUB_SCENE_H
