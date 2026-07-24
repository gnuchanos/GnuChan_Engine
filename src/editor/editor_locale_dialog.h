/**************************************************************************/
/*  editor_locale_dialog.h                                                */
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

#ifndef EDITOR_LOCALE_DIALOG_H
#define EDITOR_LOCALE_DIALOG_H

#include "core/translation.h"
#include "scene/gui/dialogs.h"

class Button;
class HBoxContainer;
class VBoxContainer;
class LineEdit;
class Tree;
class OptionButton;
class UndoRedo;

class EditorLocaleDialog : public ConfirmationDialog {
	GDCLASS(EditorLocaleDialog, ConfirmationDialog);

	enum LocaleFilter {
		SHOW_ALL_LOCALES,
		SHOW_ONLY_SELECTED_LOCALES,
	};

	HBoxContainer *hb_locale = nullptr;
	VBoxContainer *vb_script_list = nullptr;
	OptionButton *filter_mode = nullptr;
	Button *edit_filters = nullptr;
	Button *advanced = nullptr;
	LineEdit *lang_code = nullptr;
	LineEdit *script_code = nullptr;
	LineEdit *country_code = nullptr;
	LineEdit *variant_code = nullptr;
	Tree *lang_list = nullptr;
	Tree *script_list = nullptr;
	Tree *cnt_list = nullptr;

	UndoRedo *undo_redo = nullptr;

	bool locale_set = false;
	bool updating_lists = false;

protected:
	static void _bind_methods();
	virtual void _post_popup();
	virtual void ok_pressed();

	void _item_selected();
	void _filter_lang_option_changed();
	void _filter_script_option_changed();
	void _filter_cnt_option_changed();
	void _filter_mode_changed(int p_mode);
	void _edit_filters(bool p_checked);
	void _toggle_advanced(bool p_checked);

	void _update_tree();

public:
	EditorLocaleDialog();

	void set_locale(const String &p_locale);
	void popup_locale_dialog();
};

#endif // EDITOR_LOCALE_DIALOG_H
