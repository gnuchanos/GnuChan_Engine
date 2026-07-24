/**************************************************************************/
/*  panel_container.cpp                                                   */
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

#include "panel_container.h"

Size2 PanelContainer::get_minimum_size() const {
	Ref<StyleBox> style;

	if (has_stylebox("panel")) {
		style = get_stylebox("panel");
	} else {
		style = get_stylebox("panel", "PanelContainer");
	}

	Size2 ms;
	for (int i = 0; i < get_child_count(); i++) {
		Control *c = Object::cast_to<Control>(get_child(i));
		if (!c || !c->is_visible_in_tree()) {
			continue;
		}
		if (c->is_set_as_toplevel()) {
			continue;
		}

		Size2 minsize = c->get_combined_minimum_size();
		ms.width = MAX(ms.width, minsize.width);
		ms.height = MAX(ms.height, minsize.height);
	}

	if (style.is_valid()) {
		ms += style->get_minimum_size();
	}
	return ms;
}

void PanelContainer::_notification(int p_what) {
	if (p_what == NOTIFICATION_DRAW) {
		RID ci = get_canvas_item();
		Ref<StyleBox> style;

		if (has_stylebox("panel")) {
			style = get_stylebox("panel");
		} else {
			style = get_stylebox("panel", "PanelContainer");
		}

		style->draw(ci, Rect2(Point2(), get_size()));
	}

	if (p_what == NOTIFICATION_SORT_CHILDREN) {
		Ref<StyleBox> style;

		if (has_stylebox("panel")) {
			style = get_stylebox("panel");
		} else {
			style = get_stylebox("panel", "PanelContainer");
		}

		Size2 size = get_size();
		Point2 ofs;
		if (style.is_valid()) {
			size -= style->get_minimum_size();
			ofs += style->get_offset();
		}

		for (int i = 0; i < get_child_count(); i++) {
			Control *c = Object::cast_to<Control>(get_child(i));
			if (!c || !c->is_visible_in_tree()) {
				continue;
			}
			if (c->is_set_as_toplevel()) {
				continue;
			}

			fit_child_in_rect(c, Rect2(ofs, size));
		}
	}
}

PanelContainer::PanelContainer() {
}
