/**************************************************************************/
/*  viewport_container.cpp                                                */
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

#include "viewport_container.h"

#include "core/engine.h"
#include "scene/main/viewport.h"

Size2 ViewportContainer::get_minimum_size() const {
	if (stretch) {
		return Size2();
	}
	Size2 ms;
	for (int i = 0; i < get_child_count(); i++) {
		Viewport *c = Object::cast_to<Viewport>(get_child(i));
		if (!c) {
			continue;
		}

		Size2 minsize = c->get_size();
		ms.width = MAX(ms.width, minsize.width);
		ms.height = MAX(ms.height, minsize.height);
	}

	return ms;
}

void ViewportContainer::set_stretch(bool p_enable) {
	stretch = p_enable;
	minimum_size_changed();
	queue_sort();
	update();
}

bool ViewportContainer::is_stretch_enabled() const {
	return stretch;
}

void ViewportContainer::set_stretch_shrink(int p_shrink) {
	ERR_FAIL_COND(p_shrink < 1);
	if (shrink == p_shrink) {
		return;
	}

	shrink = p_shrink;

	if (!stretch) {
		return;
	}

	for (int i = 0; i < get_child_count(); i++) {
		Viewport *c = Object::cast_to<Viewport>(get_child(i));
		if (!c) {
			continue;
		}

		c->set_size(get_size() / shrink);
	}

	update();
}

int ViewportContainer::get_stretch_shrink() const {
	return shrink;
}

void ViewportContainer::_notification(int p_what) {
	if (p_what == NOTIFICATION_RESIZED) {
		if (!stretch) {
			return;
		}

		for (int i = 0; i < get_child_count(); i++) {
			Viewport *c = Object::cast_to<Viewport>(get_child(i));
			if (!c) {
				continue;
			}

			c->set_size(get_size() / shrink);
		}
	}

	if (p_what == NOTIFICATION_ENTER_TREE || p_what == NOTIFICATION_VISIBILITY_CHANGED) {
		for (int i = 0; i < get_child_count(); i++) {
			Viewport *c = Object::cast_to<Viewport>(get_child(i));
			if (!c) {
				continue;
			}

			if (is_visible_in_tree()) {
				c->set_update_mode(Viewport::UPDATE_ALWAYS);
			} else {
				c->set_update_mode(Viewport::UPDATE_DISABLED);
			}

			c->set_handle_input_locally(false); //do not handle input locally here
		}
	}

	if (p_what == NOTIFICATION_DRAW) {
		for (int i = 0; i < get_child_count(); i++) {
			Viewport *c = Object::cast_to<Viewport>(get_child(i));
			if (!c) {
				continue;
			}

			if (stretch) {
				draw_texture_rect(c->get_texture(), Rect2(Vector2(), get_size() * Size2(1, -1)));
			} else {
				draw_texture_rect(c->get_texture(), Rect2(Vector2(), c->get_size() * Size2(1, -1)));
			}
		}
	}
}

void ViewportContainer::_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	Transform2D xform = get_global_transform_with_canvas();

	if (stretch) {
		Transform2D scale_xf;
		scale_xf.scale(Vector2(shrink, shrink));
		xform *= scale_xf;
	}

	Ref<InputEvent> ev = p_event->xformed_by(xform.affine_inverse());

	for (int i = 0; i < get_child_count(); i++) {
		Viewport *c = Object::cast_to<Viewport>(get_child(i));
		if (!c || c->is_input_disabled()) {
			continue;
		}

		c->input(ev);
	}
}

void ViewportContainer::_unhandled_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	Transform2D xform = get_global_transform_with_canvas();

	if (stretch) {
		Transform2D scale_xf;
		scale_xf.scale(Vector2(shrink, shrink));
		xform *= scale_xf;
	}

	Ref<InputEvent> ev = p_event->xformed_by(xform.affine_inverse());

	for (int i = 0; i < get_child_count(); i++) {
		Viewport *c = Object::cast_to<Viewport>(get_child(i));
		if (!c || c->is_input_disabled()) {
			continue;
		}

		c->unhandled_input(ev);
	}
}

void ViewportContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_unhandled_input", "event"), &ViewportContainer::_unhandled_input);
	ClassDB::bind_method(D_METHOD("_input", "event"), &ViewportContainer::_input);
	ClassDB::bind_method(D_METHOD("set_stretch", "enable"), &ViewportContainer::set_stretch);
	ClassDB::bind_method(D_METHOD("is_stretch_enabled"), &ViewportContainer::is_stretch_enabled);

	ClassDB::bind_method(D_METHOD("set_stretch_shrink", "amount"), &ViewportContainer::set_stretch_shrink);
	ClassDB::bind_method(D_METHOD("get_stretch_shrink"), &ViewportContainer::get_stretch_shrink);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stretch"), "set_stretch", "is_stretch_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stretch_shrink", PROPERTY_HINT_RANGE, "1,32,1,or_greater"), "set_stretch_shrink", "get_stretch_shrink");
}

ViewportContainer::ViewportContainer() {
	stretch = false;
	shrink = 1;
	set_process_input(true);
	set_process_unhandled_input(true);
}
