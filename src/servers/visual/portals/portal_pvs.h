/**************************************************************************/
/*  portal_pvs.h                                                          */
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

#ifndef PORTAL_PVS_H
#define PORTAL_PVS_H

#include "core/local_vector.h"

class PVS {
public:
	void clear();

	void add_to_pvs(int p_room_id) { _room_pvs.push_back(p_room_id); }
	int32_t get_pvs_size() const { return _room_pvs.size(); }
	int32_t get_pvs_room_id(int32_t p_entry) const { return _room_pvs[p_entry]; }

	void add_to_secondary_pvs(int p_room_id) { _room_secondary_pvs.push_back(p_room_id); }
	int32_t get_secondary_pvs_size() const { return _room_secondary_pvs.size(); }
	int32_t get_secondary_pvs_room_id(int32_t p_entry) const { return _room_secondary_pvs[p_entry]; }

	void set_loaded(bool p_loaded) { _loaded = p_loaded; }
	bool is_loaded() const { return _loaded; }

private:
	// pvs
	LocalVector<uint16_t, int32_t> _room_pvs;
	// secondary pvs is primary plus the immediate neighbors of the primary pvs
	LocalVector<uint16_t, int32_t> _room_secondary_pvs;
	bool _loaded = false;
};

#endif // PORTAL_PVS_H
