/**************************************************************************/
/*  listener.cpp                                                          */
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

#include "listener.h"

#include "scene/resources/mesh.h"

void Listener::_update_audio_listener_state() {
}

void Listener::_request_listener_update() {
	_update_listener();
}

bool Listener::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "current") {
		if (p_value.operator bool()) {
			make_current();
		} else {
			clear_current();
		}
	} else {
		return false;
	}

	return true;
}
bool Listener::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "current") {
		if (is_inside_tree() && get_tree()->is_node_being_edited(this)) {
			r_ret = current;
		} else {
			r_ret = is_current();
		}
	} else {
		return false;
	}

	return true;
}

void Listener::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::BOOL, PNAME("current")));
}

void Listener::_update_listener() {
	if (is_inside_tree() && is_current()) {
		get_viewport()->_listener_transform_changed_notify();
	}
}

void Listener::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_WORLD: {
			bool first_listener = get_viewport()->_listener_add(this);
			if (!get_tree()->is_node_being_edited(this) && (current || first_listener)) {
				make_current();
			}
		} break;
		case NOTIFICATION_TRANSFORM_CHANGED: {
			_request_listener_update();
		} break;
		case NOTIFICATION_EXIT_WORLD: {
			if (!get_tree()->is_node_being_edited(this)) {
				if (is_current()) {
					clear_current();
					current = true; //keep it true

				} else {
					current = false;
				}
			}

			get_viewport()->_listener_remove(this);

		} break;
	}
}

Transform Listener::get_listener_transform() const {
	return get_global_transform().orthonormalized();
}

void Listener::make_current() {
	current = true;

	if (!is_inside_tree()) {
		return;
	}

	get_viewport()->_listener_set(this);
}

void Listener::clear_current() {
	current = false;
	if (!is_inside_tree()) {
		return;
	}

	if (get_viewport()->get_listener() == this) {
		get_viewport()->_listener_set(nullptr);
		get_viewport()->_listener_make_next_current(this);
	}
}

bool Listener::is_current() const {
	if (is_inside_tree() && !get_tree()->is_node_being_edited(this)) {
		return get_viewport()->get_listener() == this;
	} else {
		return current;
	}

	return false;
}

void Listener::_bind_methods() {
	ClassDB::bind_method(D_METHOD("make_current"), &Listener::make_current);
	ClassDB::bind_method(D_METHOD("clear_current"), &Listener::clear_current);
	ClassDB::bind_method(D_METHOD("is_current"), &Listener::is_current);
	ClassDB::bind_method(D_METHOD("get_listener_transform"), &Listener::get_listener_transform);
}

Listener::Listener() {
	current = false;
	force_change = false;
	set_notify_transform(true);
}

Listener::~Listener() {
}
