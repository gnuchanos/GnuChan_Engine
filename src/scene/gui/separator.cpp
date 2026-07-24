/**************************************************************************/
/*  separator.cpp                                                         */
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

#include "separator.h"

Size2 Separator::get_minimum_size() const {
	Size2 ms(3, 3);
	if (orientation == VERTICAL) {
		ms.x = get_constant("separation");
	} else { // HORIZONTAL
		ms.y = get_constant("separation");
	}
	return ms;
}

void Separator::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			Size2i size = get_size();
			Ref<StyleBox> style = get_stylebox("separator");
			Size2i ssize = style->get_minimum_size() + style->get_center_size();

			if (orientation == VERTICAL) {
				style->draw(get_canvas_item(), Rect2((size.x - ssize.x) / 2, 0, ssize.x, size.y));
			} else {
				style->draw(get_canvas_item(), Rect2(0, (size.y - ssize.y) / 2, size.x, ssize.y));
			}

		} break;
	}
}

Separator::Separator() {
}

Separator::~Separator() {
}

HSeparator::HSeparator() {
	orientation = HORIZONTAL;
}

VSeparator::VSeparator() {
	orientation = VERTICAL;
}
