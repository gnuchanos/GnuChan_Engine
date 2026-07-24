/**************************************************************************/
/*  navigation_obstacle_2d.h                                              */
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

#ifndef NAVIGATION_OBSTACLE_2D_H
#define NAVIGATION_OBSTACLE_2D_H

#include "scene/2d/node_2d.h"
#include "scene/main/node.h"

class Navigation2D;

class NavigationObstacle2D : public Node {
	GDCLASS(NavigationObstacle2D, Node);

	Navigation2D *navigation;
	Node2D *parent_node2d = nullptr;
	RID agent;
	RID map_before_pause;

	bool estimate_radius = true;
	real_t radius = 1.0;

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &property) const;
	void _notification(int p_what);

public:
	NavigationObstacle2D();
	virtual ~NavigationObstacle2D();

	void set_navigation(Navigation2D *p_nav);
	const Navigation2D *get_navigation() const {
		return navigation;
	}

	void set_navigation_node(Node *p_nav);
	Node *get_navigation_node() const;

	RID get_rid() const {
		return agent;
	}

	void set_estimate_radius(bool p_estimate_radius);
	bool is_radius_estimated() const {
		return estimate_radius;
	}
	void set_radius(real_t p_radius);
	real_t get_radius() const {
		return radius;
	}
	virtual String get_configuration_warning() const;

private:
	void initialize_agent();
	void reevaluate_agent_radius();
	real_t estimate_agent_radius() const;
};

#endif // NAVIGATION_OBSTACLE_2D_H
