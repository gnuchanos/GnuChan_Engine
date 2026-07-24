/**************************************************************************/
/*  editor_path.h                                                         */
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

#ifndef EDITOR_PATH_H
#define EDITOR_PATH_H

#include "editor_data.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/popup_menu.h"

class EditorPath : public Button {
	GDCLASS(EditorPath, Button);

	EditorHistory *history;

	TextureRect *current_object_icon;
	Label *current_object_label;
	TextureRect *sub_objects_icon;
	PopupMenu *sub_objects_menu;

	Vector<ObjectID> objects;

	void _show_popup();
	void _id_pressed(int p_idx);
	void _about_to_show();
	void _add_children_to_popup(Object *p_obj, int p_depth = 0);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void update_path();
	void clear_path();
	void enable_path();

	EditorPath(EditorHistory *p_history);
};

#endif // EDITOR_PATH_H
