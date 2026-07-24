/**************************************************************************/
/*  shortcut.cpp                                                          */
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

#include "shortcut.h"

#include "core/os/keyboard.h"

void ShortCut::set_shortcut(const Ref<InputEvent> &p_shortcut) {
	shortcut = p_shortcut;
	emit_changed();
}

Ref<InputEvent> ShortCut::get_shortcut() const {
	return shortcut;
}

bool ShortCut::is_shortcut(const Ref<InputEvent> &p_event) const {
	return shortcut.is_valid() && shortcut->shortcut_match(p_event);
}

String ShortCut::get_as_text() const {
	if (shortcut.is_valid()) {
		return shortcut->as_text();
	} else {
		return "None";
	}
}

bool ShortCut::is_valid() const {
	return shortcut.is_valid();
}

void ShortCut::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_shortcut", "event"), &ShortCut::set_shortcut);
	ClassDB::bind_method(D_METHOD("get_shortcut"), &ShortCut::get_shortcut);

	ClassDB::bind_method(D_METHOD("is_valid"), &ShortCut::is_valid);

	ClassDB::bind_method(D_METHOD("is_shortcut", "event"), &ShortCut::is_shortcut);
	ClassDB::bind_method(D_METHOD("get_as_text"), &ShortCut::get_as_text);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shortcut", PROPERTY_HINT_RESOURCE_TYPE, "InputEvent"), "set_shortcut", "get_shortcut");
}

ShortCut::ShortCut() {
}
