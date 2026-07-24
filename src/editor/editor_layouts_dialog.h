/**************************************************************************/
/*  editor_layouts_dialog.h                                               */
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

#ifndef EDITOR_LAYOUTS_DIALOG_H
#define EDITOR_LAYOUTS_DIALOG_H

#include "scene/gui/dialogs.h"

class LineEdit;
class ItemList;

class EditorLayoutsDialog : public ConfirmationDialog {
	GDCLASS(EditorLayoutsDialog, ConfirmationDialog);

	LineEdit *name;
	ItemList *layout_names;
	VBoxContainer *makevb;

	void _line_gui_input(const Ref<InputEvent> &p_event);

protected:
	static void _bind_methods();
	virtual void ok_pressed();
	virtual void _post_popup();

public:
	EditorLayoutsDialog();

	void set_name_line_enabled(bool p_enabled);
};

#endif // EDITOR_LAYOUTS_DIALOG_H
