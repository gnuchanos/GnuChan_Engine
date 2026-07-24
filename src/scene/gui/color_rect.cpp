/**************************************************************************/
/*  color_rect.cpp                                                        */
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

#include "color_rect.h"

void ColorRect::set_frame_color(const Color &p_color) {
	color = p_color;
	update();
}

Color ColorRect::get_frame_color() const {
	return color;
}

void ColorRect::_notification(int p_what) {
	if (p_what == NOTIFICATION_DRAW) {
		draw_rect(Rect2(Point2(), get_size()), color);
	}
}

void ColorRect::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_frame_color", "color"), &ColorRect::set_frame_color);
	ClassDB::bind_method(D_METHOD("get_frame_color"), &ColorRect::get_frame_color);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_frame_color", "get_frame_color");
}

ColorRect::ColorRect() {
	color = Color(1, 1, 1);
}
