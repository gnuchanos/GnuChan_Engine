/**************************************************************************/
/*  rvo_agent.h                                                           */
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

#ifndef RVO_AGENT_H
#define RVO_AGENT_H

#include "core/object.h"
#include "nav_rid.h"

#include <Agent.h>

class NavMap;

class RvoAgent : public NavRid {
	struct AvoidanceComputedCallback {
		ObjectID id;
		StringName method;
		Variant udata;
		Variant new_velocity;
	};

	NavMap *map = nullptr;
	RVO::Agent agent;
	AvoidanceComputedCallback callback;
	uint32_t map_update_id = 0;

public:
	RvoAgent();

	void set_map(NavMap *p_map);
	NavMap *get_map() {
		return map;
	}

	RVO::Agent *get_agent() {
		return &agent;
	}

	bool is_map_changed();

	void set_callback(ObjectID p_id, const StringName p_method, const Variant p_udata = Variant());
	bool has_callback() const;

	void dispatch_callback();
};

#endif // RVO_AGENT_H
