/**************************************************************************/
/*  rvo_agent.cpp                                                         */
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

#include "rvo_agent.h"

#include "nav_map.h"

RvoAgent::RvoAgent() {
	callback.id = ObjectID(0);
}

void RvoAgent::set_map(NavMap *p_map) {
	map = p_map;
}

bool RvoAgent::is_map_changed() {
	if (map) {
		bool is_changed = map->get_map_update_id() != map_update_id;
		map_update_id = map->get_map_update_id();
		return is_changed;
	} else {
		return false;
	}
}

void RvoAgent::set_callback(ObjectID p_id, const StringName p_method, const Variant p_udata) {
	callback.id = p_id;
	callback.method = p_method;
	callback.udata = p_udata;
}

bool RvoAgent::has_callback() const {
	return callback.id != 0;
}

void RvoAgent::dispatch_callback() {
	if (callback.id == 0) {
		return;
	}
	Object *obj = ObjectDB::get_instance(callback.id);
	if (!obj) {
		callback.id = ObjectID(0);
		return;
	}

	Variant::CallError responseCallError;

	callback.new_velocity = Vector3(agent.newVelocity_.x(), agent.newVelocity_.y(), agent.newVelocity_.z());

	const Variant *vp[2] = { &callback.new_velocity, &callback.udata };
	int argc = (callback.udata.get_type() == Variant::NIL) ? 1 : 2;
	obj->call(callback.method, vp, argc, responseCallError);
}
