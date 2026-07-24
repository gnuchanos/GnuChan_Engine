/**************************************************************************/
/*  directory_create_dialog.h                                             */
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

#ifndef DIRECTORY_CREATE_DIALOG_H
#define DIRECTORY_CREATE_DIALOG_H

#include "scene/gui/dialogs.h"

class Label;
class LineEdit;
class PanelContainer;

class DirectoryCreateDialog : public ConfirmationDialog {
	GDCLASS(DirectoryCreateDialog, ConfirmationDialog);

	String base_dir;

	Label *label = nullptr;
	LineEdit *dir_path = nullptr;

	PanelContainer *status_panel = nullptr;
	Label *status_label = nullptr;

	String _validate_path(const String &p_path) const;

	void _on_dir_path_changed(const String &p_text);

protected:
	static void _bind_methods();
	void _notification(int p_what);

	virtual void ok_pressed();
	virtual void _post_popup();

public:
	void config(const String &p_base_dir);

	DirectoryCreateDialog();
};

#endif // DIRECTORY_CREATE_DIALOG_H
