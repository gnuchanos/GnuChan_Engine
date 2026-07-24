/**************************************************************************/
/*  margin_container.cpp                                                  */
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

#include "margin_container.h"

Size2 MarginContainer::get_minimum_size() const {
	int margin_left = get_constant("margin_left");
	int margin_top = get_constant("margin_top");
	int margin_right = get_constant("margin_right");
	int margin_bottom = get_constant("margin_bottom");

	Size2 max;

	for (int i = 0; i < get_child_count(); i++) {
		Control *c = Object::cast_to<Control>(get_child(i));
		if (!c) {
			continue;
		}
		if (c->is_set_as_toplevel()) {
			continue;
		}
		if (!c->is_visible()) {
			continue;
		}

		Size2 s = c->get_combined_minimum_size();
		if (s.width > max.width) {
			max.width = s.width;
		}
		if (s.height > max.height) {
			max.height = s.height;
		}
	}

	max.width += (margin_left + margin_right);
	max.height += (margin_top + margin_bottom);

	return max;
}

void MarginContainer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_SORT_CHILDREN: {
			int margin_left = get_constant("margin_left");
			int margin_top = get_constant("margin_top");
			int margin_right = get_constant("margin_right");
			int margin_bottom = get_constant("margin_bottom");

			Size2 s = get_size();

			for (int i = 0; i < get_child_count(); i++) {
				Control *c = Object::cast_to<Control>(get_child(i));
				if (!c) {
					continue;
				}
				if (c->is_set_as_toplevel()) {
					continue;
				}

				int w = s.width - margin_left - margin_right;
				int h = s.height - margin_top - margin_bottom;
				fit_child_in_rect(c, Rect2(margin_left, margin_top, w, h));
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			minimum_size_changed();
		} break;
	}
}

MarginContainer::MarginContainer() {
}
