/**************************************************************************/
/*  cull_instance.h                                                       */
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

#ifndef CULL_INSTANCE_H
#define CULL_INSTANCE_H

#include "scene/3d/spatial.h"

class CullInstance : public Spatial {
	GDCLASS(CullInstance, Spatial);

public:
	enum PortalMode : unsigned int {
		PORTAL_MODE_STATIC, // not moving within a room
		PORTAL_MODE_DYNAMIC, //  moving within room
		PORTAL_MODE_ROAMING, // moving between rooms
		PORTAL_MODE_GLOBAL, // frustum culled only
		PORTAL_MODE_IGNORE, // don't show at all - e.g. manual bounds, hidden portals
	};

	void set_portal_mode(CullInstance::PortalMode p_mode);
	CullInstance::PortalMode get_portal_mode() const;

	void set_include_in_bound(bool p_enabled) { _include_in_bound = p_enabled; }
	bool get_include_in_bound() const { return _include_in_bound; }

	void set_allow_merging(bool p_enabled) { _allow_merging = p_enabled; }
	bool get_allow_merging() const { return _allow_merging; }

	void set_portal_autoplace_priority(int p_priority) { _portal_autoplace_priority = p_priority; }
	int get_portal_autoplace_priority() const { return _portal_autoplace_priority; }

	CullInstance();

protected:
	virtual void _refresh_portal_mode() = 0;

	static void _bind_methods();

private:
	PortalMode _portal_mode;
	bool _include_in_bound : 1;
	bool _allow_merging : 1;

	// Allows instances to prefer to be autoplaced
	// in specific RoomGroups. This allows building exteriors
	// to be autoplaced in outside RoomGroups, allowing a complete
	// exterior / interior of building in one reusable Scene.
	// The default value 0 gives no preference (chooses the highest priority).
	// All other values will autoplace in the selected RoomGroup priority by preference.
	int _portal_autoplace_priority;
};

#endif // CULL_INSTANCE_H
