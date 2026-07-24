/**************************************************************************/
/*  editor_dir_dialog.h                                                   */
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

#ifndef EDITOR_DIR_DIALOG_H
#define EDITOR_DIR_DIALOG_H

#include "core/os/dir_access.h"
#include "editor/editor_file_system.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"

class EditorDirDialog : public ConfirmationDialog {
	GDCLASS(EditorDirDialog, ConfirmationDialog);

	ConfirmationDialog *makedialog;
	LineEdit *makedirname;
	AcceptDialog *mkdirerr;

	Button *makedir;
	Set<String> opened_paths;

	Tree *tree;
	bool updating;

	void _item_collapsed(Object *p_item);
	void _update_dir(TreeItem *p_item, EditorFileSystemDirectory *p_dir, const String &p_select_path = String());

	void _make_dir();
	void _make_dir_confirm();

	void ok_pressed();

	bool must_reload;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void reload(const String &p_path = "");
	EditorDirDialog();
};

#endif // EDITOR_DIR_DIALOG_H
