/**************************************************************************/
/*  y_sort.cpp                                                            */
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

#include "y_sort.h"

void YSort::set_sort_enabled(bool p_enabled) {
	sort_enabled = p_enabled;
	VS::get_singleton()->canvas_item_set_sort_children_by_y(get_canvas_item(), sort_enabled);
}

bool YSort::is_sort_enabled() const {
	return sort_enabled;
}

void YSort::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sort_enabled", "enabled"), &YSort::set_sort_enabled);
	ClassDB::bind_method(D_METHOD("is_sort_enabled"), &YSort::is_sort_enabled);

	ADD_GROUP("Sort", "sort_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "sort_enabled"), "set_sort_enabled", "is_sort_enabled");
}

YSort::YSort() {
	sort_enabled = true;
	VS::get_singleton()->canvas_item_set_sort_children_by_y(get_canvas_item(), true);
}
