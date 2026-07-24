/**************************************************************************/
/*  navigation_agent.h                                                    */
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

#ifndef NAVIGATION_AGENT_H
#define NAVIGATION_AGENT_H

#include "core/vector.h"
#include "scene/main/node.h"

class Spatial;
class Navigation;

class NavigationAgent : public Node {
	GDCLASS(NavigationAgent, Node);

	Spatial *agent_parent = nullptr;
	Navigation *navigation = nullptr;

	RID agent;
	RID map_before_pause;
	RID map_override;

	bool avoidance_enabled = false;
	uint32_t navigation_layers = 1;

	real_t path_desired_distance = 1.0;
	real_t target_desired_distance = 1.0;
	real_t radius = 0.0;
	real_t navigation_height_offset = 0.0;
	bool ignore_y = false;
	real_t neighbor_dist = 0.0;
	int max_neighbors = 0;
	real_t time_horizon = 0.0;
	real_t max_speed = 0.0;

	real_t path_max_distance = 3.0;

	Vector3 target_location;
	Vector<Vector3> navigation_path;
	int nav_path_index = 0;
	bool velocity_submitted = false;
	Vector3 prev_safe_velocity;
	/// The submitted target velocity
	Vector3 target_velocity;
	bool target_reached = false;
	bool navigation_finished = true;
	// No initialized on purpose
	uint32_t update_frame_id = 0;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	NavigationAgent();
	virtual ~NavigationAgent();

	void set_navigation(Navigation *p_nav);
	const Navigation *get_navigation() const {
		return navigation;
	}

	void set_navigation_node(Node *p_nav);
	Node *get_navigation_node() const;

	RID get_rid() const {
		return agent;
	}

	void set_avoidance_enabled(bool p_enabled);
	bool get_avoidance_enabled() const;

	void set_agent_parent(Node *p_agent_parent);

	void set_navigation_layers(uint32_t p_navigation_layers);
	uint32_t get_navigation_layers() const;

	void set_navigation_map(RID p_navigation_map);
	RID get_navigation_map() const;

	void set_path_desired_distance(real_t p_dd);
	real_t get_path_desired_distance() const {
		return path_desired_distance;
	}

	void set_target_desired_distance(real_t p_dd);
	real_t get_target_desired_distance() const {
		return target_desired_distance;
	}

	void set_radius(real_t p_radius);
	real_t get_radius() const {
		return radius;
	}

	void set_agent_height_offset(real_t p_hh);
	real_t get_agent_height_offset() const {
		return navigation_height_offset;
	}

	void set_ignore_y(bool p_ignore_y);
	bool get_ignore_y() const {
		return ignore_y;
	}

	void set_neighbor_dist(real_t p_dist);
	real_t get_neighbor_dist() const {
		return neighbor_dist;
	}

	void set_max_neighbors(int p_count);
	int get_max_neighbors() const {
		return max_neighbors;
	}

	void set_time_horizon(real_t p_time);
	real_t get_time_horizon() const {
		return time_horizon;
	}

	void set_max_speed(real_t p_max_speed);
	real_t get_max_speed() const {
		return max_speed;
	}

	void set_path_max_distance(real_t p_pmd);
	real_t get_path_max_distance();

	void set_target_location(Vector3 p_location);
	Vector3 get_target_location() const;

	Vector3 get_next_location();

	Vector<Vector3> get_nav_path() const {
		return navigation_path;
	}

	int get_nav_path_index() const {
		return nav_path_index;
	}

	real_t distance_to_target() const;
	bool is_target_reached() const;
	bool is_target_reachable();
	bool is_navigation_finished();
	Vector3 get_final_location();

	void set_velocity(Vector3 p_velocity);
	void _avoidance_done(Vector3 p_new_velocity);

	virtual String get_configuration_warning() const;

private:
	void update_navigation();
	void _request_repath();
	void _check_distance_to_target();
};

#endif // NAVIGATION_AGENT_H
