/**************************************************************************/
/*  editor_quick_open.h                                                   */
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

#ifndef EDITOR_QUICK_OPEN_H
#define EDITOR_QUICK_OPEN_H

#include "core/pair.h"
#include "editor_file_system.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/tree.h"

class EditorQuickOpen : public ConfirmationDialog {
	GDCLASS(EditorQuickOpen, ConfirmationDialog);

	LineEdit *search_box;
	Tree *search_options;
	StringName base_type;
	StringName ei;
	StringName ot;

	void _update_search();

	void _sbox_input(const Ref<InputEvent> &p_ie);
	void _parse_fs(EditorFileSystemDirectory *efsd, Vector<Pair<String, Ref<Texture>>> &list);
	Vector<Pair<String, Ref<Texture>>> _sort_fs(Vector<Pair<String, Ref<Texture>>> &list);
	float _path_cmp(String search, String path) const;

	void _confirmed();
	void _text_changed(const String &p_newtext);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	StringName get_base_type() const;

	String get_selected() const;
	Vector<String> get_selected_files() const;

	void popup_dialog(const StringName &p_base, bool p_enable_multi = false, bool p_dontclear = false);
	EditorQuickOpen();
};

#endif // EDITOR_QUICK_OPEN_H
