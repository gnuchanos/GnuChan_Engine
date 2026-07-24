/**************************************************************************/
/*  scene_create_dialog.h                                                 */
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

#ifndef SCENE_CREATE_DIALOG_H
#define SCENE_CREATE_DIALOG_H

#include "scene/gui/dialogs.h"

class ButtonGroup;
class CheckBox;
class CreateDialog;
class EditorFileDialog;
class Label;
class LineEdit;
class OptionButton;
class PanelContainer;

class SceneCreateDialog : public ConfirmationDialog {
	GDCLASS(SceneCreateDialog, ConfirmationDialog);

	enum MsgType {
		MSG_OK,
		MSG_ERROR,
	};

	const StringName type_meta = StringName("type");

public:
	enum RootType {
		ROOT_2D_SCENE,
		ROOT_3D_SCENE,
		ROOT_USER_INTERFACE,
		ROOT_OTHER,
	};

private:
	String directory;
	String scene_name;
	String root_name;

	Ref<ButtonGroup> node_type_group;
	CheckBox *node_type_2d = nullptr;
	CheckBox *node_type_3d = nullptr;
	CheckBox *node_type_gui = nullptr;
	CheckBox *node_type_other = nullptr;

	LineEdit *other_type_display = nullptr;
	Button *select_node_button = nullptr;
	CreateDialog *select_node_dialog = nullptr;

	LineEdit *scene_name_edit = nullptr;
	OptionButton *scene_extension_picker = nullptr;
	LineEdit *root_name_edit = nullptr;

	PanelContainer *status_panel = nullptr;
	Label *file_error_label = nullptr;
	Label *node_error_label = nullptr;

	void accept_create(Variant p_discard = Variant()); // Extra unused argument, because unbind() doesn't exist in 3.x.
	void browse_types();
	void on_type_picked();
	void update_dialog(Variant p_discard = Variant());
	void update_error(Label *p_label, MsgType p_type, const String &p_msg);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void config(const String &p_dir);

	String get_scene_path() const;
	Node *create_scene_root();

	SceneCreateDialog();
};

#endif // SCENE_CREATE_DIALOG_H
