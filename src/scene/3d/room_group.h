/**************************************************************************/
/*  room_group.h                                                          */
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

#ifndef ROOM_GROUP_H
#define ROOM_GROUP_H

#include "core/rid.h"
#include "spatial.h"

class Room;

class RoomGroup : public Spatial {
	GDCLASS(RoomGroup, Spatial);

	friend class RoomManager;

	RID _room_group_rid;

public:
	RoomGroup();
	~RoomGroup();

	void add_room(Room *p_room);

	void set_roomgroup_priority(int p_priority) {
		_settings_priority = p_priority;
		_changed();
	}
	int get_roomgroup_priority() const { return _settings_priority; }

	String get_configuration_warning() const;

private:
	void clear();
	void _changed();

	// roomgroup ID during conversion
	int _roomgroup_ID;

	// the roomgroup can be used to set a number of rooms to a different priority
	// to allow a group of rooms WITHIN another room / rooms.
	// This is for e.g. buildings on landscape.
	int _settings_priority = 0;

	// makes sure lrooms are not converted more than once per
	// call to rooms_convert
	int _conversion_tick = -1;

protected:
	static void _bind_methods();
	void _notification(int p_what);
};

#endif // ROOM_GROUP_H
