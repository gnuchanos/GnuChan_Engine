/**************************************************************************/
/*  pane_drag.cpp                                                         */
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

#include "pane_drag.h"

void PaneDrag::_gui_input(const Ref<InputEvent> &p_input) {
	Ref<InputEventMouseMotion> mm = p_input;
	if (mm.is_valid() && mm->get_button_mask() & BUTTON_MASK_LEFT) {
		emit_signal("dragged", Point2(mm->get_relative().x, mm->get_relative().y));
	}
}

void PaneDrag::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			Ref<Texture> icon = mouse_over ? get_icon("PaneDragHover", "EditorIcons") : get_icon("PaneDrag", "EditorIcons");
			if (!icon.is_null()) {
				icon->draw(get_canvas_item(), Point2(0, 0));
			}

		} break;
		case NOTIFICATION_MOUSE_ENTER:
			mouse_over = true;
			update();
			break;
		case NOTIFICATION_MOUSE_EXIT:
			mouse_over = false;
			update();
			break;
	}
}
Size2 PaneDrag::get_minimum_size() const {
	Ref<Texture> icon = get_icon("PaneDrag", "EditorIcons");
	if (!icon.is_null()) {
		return icon->get_size();
	}
	return Size2();
}

void PaneDrag::_bind_methods() {
	ClassDB::bind_method("_gui_input", &PaneDrag::_gui_input);
	ADD_SIGNAL(MethodInfo("dragged", PropertyInfo(Variant::VECTOR2, "amount")));
}

PaneDrag::PaneDrag() {
	mouse_over = false;
}
