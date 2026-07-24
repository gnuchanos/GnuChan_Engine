/**************************************************************************/
/*  panel.cpp                                                             */
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

#include "panel.h"
#include "core/print_string.h"

void Panel::_notification(int p_what) {
	if (p_what == NOTIFICATION_DRAW) {
		RID ci = get_canvas_item();
		Ref<StyleBox> style = get_stylebox("panel");
		style->draw(ci, Rect2(Point2(), get_size()));
	}
}

Panel::Panel() {
	set_mouse_filter(MOUSE_FILTER_STOP);
}

Panel::~Panel() {
}
