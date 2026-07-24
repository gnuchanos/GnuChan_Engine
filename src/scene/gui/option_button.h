/**************************************************************************/
/*  option_button.h                                                       */
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

#ifndef OPTION_BUTTON_H
#define OPTION_BUTTON_H

#include "scene/gui/button.h"
#include "scene/gui/popup_menu.h"

class OptionButton : public Button {
	GDCLASS(OptionButton, Button);

	PopupMenu *popup;
	int current;

	void _focused(int p_which);
	void _selected(int p_which);
	void _select(int p_which, bool p_emit = false);
	void _select_int(int p_which);

	Array _get_items() const;
	void _set_items(const Array &p_items);

	virtual void pressed();

protected:
	Size2 get_minimum_size() const;
	void _notification(int p_what);
	static void _bind_methods();

public:
	void add_icon_item(const Ref<Texture> &p_icon, const String &p_label, int p_id = -1);
	void add_item(const String &p_label, int p_id = -1);

	void set_item_text(int p_idx, const String &p_text);
	void set_item_icon(int p_idx, const Ref<Texture> &p_icon);
	void set_item_id(int p_idx, int p_id);
	void set_item_metadata(int p_idx, const Variant &p_metadata);
	void set_item_disabled(int p_idx, bool p_disabled);
	void set_item_tooltip(int p_idx, const String &p_tooltip);

	String get_item_text(int p_idx) const;
	Ref<Texture> get_item_icon(int p_idx) const;
	int get_item_id(int p_idx) const;
	int get_item_index(int p_id) const;
	Variant get_item_metadata(int p_idx) const;
	bool is_item_disabled(int p_idx) const;
	String get_item_tooltip(int p_idx) const;

	int get_item_count() const;

	void add_separator();

	void clear();

	void select(int p_idx);
	int get_selected() const;
	int get_selected_id() const;
	Variant get_selected_metadata() const;

	void remove_item(int p_idx);

	PopupMenu *get_popup() const;

	virtual void get_translatable_strings(List<String> *p_strings) const;

	OptionButton();
	~OptionButton();
};

#endif // OPTION_BUTTON_H
