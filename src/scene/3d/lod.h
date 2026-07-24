/**************************************************************************/
/*  lod.h                                                                 */
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

#ifndef LOD_H
#define LOD_H

#include "spatial.h"

class LOD : public Spatial {
	GDCLASS(LOD, Spatial);

	struct LODChild {
		float distance;
		int32_t child_id;
	};

	struct Data {
		LocalVector<LODChild> lod_children;
		int32_t current_lod_child = 0;
		float hysteresis = 1.0f;
		int32_t queue_id = 0;
		const Spatial *current_lod_node = nullptr;
		bool registered = false;
	} data;

	friend class LODManager;
	bool _lod_update(float p_camera_dist_squared);
	void _lod_pre_save();
	void _update_child_distances();

	void _lod_register();
	void _lod_unregister();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_hysteresis(real_t p_distance);
	real_t get_hysteresis() const { return data.hysteresis; }

	void set_lod_priority(int p_priority);
	int get_lod_priority() const { return data.queue_id; }
};

#endif // LOD_H
