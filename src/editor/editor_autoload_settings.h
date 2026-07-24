/**************************************************************************/
/*  editor_autoload_settings.h                                            */
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

#ifndef EDITOR_AUTOLOAD_SETTINGS_H
#define EDITOR_AUTOLOAD_SETTINGS_H

#include "scene/gui/tree.h"

#include "editor_file_dialog.h"

class EditorAutoloadSettings : public VBoxContainer {
	GDCLASS(EditorAutoloadSettings, VBoxContainer);

	enum {
		BUTTON_OPEN,
		BUTTON_MOVE_UP,
		BUTTON_MOVE_DOWN,
		BUTTON_DELETE
	};

	String autoload_changed;

	struct AutoLoadInfo {
		String name;
		String path;
		bool is_singleton;
		bool in_editor;
		int order;
		Node *node;

		bool operator==(const AutoLoadInfo &p_info) const {
			return order == p_info.order;
		}

		AutoLoadInfo() {
			is_singleton = false;
			in_editor = false;
			node = nullptr;
		}
	};

	List<AutoLoadInfo> autoload_cache;

	bool updating_autoload;
	int number_of_autoloads;
	String selected_autoload;

	Tree *tree;
	EditorLineEditFileChooser *autoload_add_path;
	LineEdit *autoload_add_name;
	Button *add_autoload;
	Label *error_message;

	bool _autoload_name_is_valid(const String &p_name, String *r_error = nullptr);

	void _autoload_add();
	void _autoload_selected();
	void _autoload_edited();
	void _autoload_button_pressed(Object *p_item, int p_column, int p_button);
	void _autoload_activated();
	void _autoload_path_text_changed(const String p_path);
	void _autoload_text_entered(const String p_name);
	void _autoload_text_changed(const String p_name);
	void _autoload_open(const String &fpath);
	void _autoload_file_callback(const String &p_path);
	Node *_create_autoload(const String &p_path);

	Variant get_drag_data_fw(const Point2 &p_point, Control *p_control);
	bool can_drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_control) const;
	void drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_control);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void update_autoload();
	bool autoload_add(const String &p_name, const String &p_path);
	void autoload_remove(const String &p_name);

	EditorAutoloadSettings();
	~EditorAutoloadSettings();
};

#endif // EDITOR_AUTOLOAD_SETTINGS_H
