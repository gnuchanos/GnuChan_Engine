/**************************************************************************/
/*  spatial_velocity_tracker.h                                            */
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

#ifndef SPATIAL_VELOCITY_TRACKER_H
#define SPATIAL_VELOCITY_TRACKER_H

#include "scene/3d/spatial.h"

class SpatialVelocityTracker : public Reference {
	GDCLASS(SpatialVelocityTracker, Reference);

	struct PositionHistory {
		uint64_t frame;
		Vector3 position;
	};

	bool physics_step;
	Vector<PositionHistory> position_history;
	int position_history_len;

protected:
	static void _bind_methods();

public:
	void reset(const Vector3 &p_new_pos);
	void set_track_physics_step(bool p_track_physics_step);
	bool is_tracking_physics_step() const;
	void update_position(const Vector3 &p_position);
	Vector3 get_tracked_linear_velocity() const;

	SpatialVelocityTracker();
};

#endif // SPATIAL_VELOCITY_TRACKER_H
