/**************************************************************************/
/*  menu_button.h                                                         */
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

#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include "scene/gui/button.h"
#include "scene/gui/popup_menu.h"

class MenuButton : public Button {
	GDCLASS(MenuButton, Button);

	bool clicked;
	bool switch_on_hover;
	bool disable_shortcuts;
	PopupMenu *popup;

	void _unhandled_key_input(Ref<InputEvent> p_event);
	Array _get_items() const;
	void _set_items(const Array &p_items);

	void _gui_input(Ref<InputEvent> p_event);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	virtual void pressed();

	PopupMenu *get_popup() const;
	void set_switch_on_hover(bool p_enabled);
	bool is_switch_on_hover();
	void set_disable_shortcuts(bool p_disabled);

	MenuButton();
	~MenuButton();
};

#endif // MENU_BUTTON_H
