/**************************************************************************/
/*  nav_region.h                                                          */
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

#ifndef NAV_REGION_H
#define NAV_REGION_H

#include "scene/3d/navigation.h"

#include "nav_rid.h"
#include "nav_utils.h"

#include <vector>

class NavMap;
class NavRegion;

class NavRegion : public NavRid {
	NavMap *map = nullptr;
	Transform transform;
	Ref<NavigationMesh> mesh;
	uint32_t navigation_layers = 1;
	float enter_cost = 0.0;
	float travel_cost = 1.0;
	Vector<gd::Edge::Connection> connections;

	bool polygons_dirty = true;

	/// Cache
	LocalVector<gd::Polygon> polygons;

public:
	void scratch_polygons() {
		polygons_dirty = true;
	}

	void set_map(NavMap *p_map);
	NavMap *get_map() const {
		return map;
	}

	void set_enter_cost(float p_enter_cost) { enter_cost = MAX(p_enter_cost, 0.0); }
	float get_enter_cost() const { return enter_cost; }

	void set_travel_cost(float p_travel_cost) { travel_cost = MAX(p_travel_cost, 0.0); }
	float get_travel_cost() const { return travel_cost; }

	void set_navigation_layers(uint32_t p_navigation_layers);
	uint32_t get_navigation_layers() const;

	void set_transform(Transform transform);
	const Transform &get_transform() const {
		return transform;
	}

	void set_mesh(Ref<NavigationMesh> p_mesh);
	const Ref<NavigationMesh> get_mesh() const {
		return mesh;
	}

	Vector<gd::Edge::Connection> &get_connections() {
		return connections;
	}
	int get_connections_count() const;
	Vector3 get_connection_pathway_start(int p_connection_id) const;
	Vector3 get_connection_pathway_end(int p_connection_id) const;

	LocalVector<gd::Polygon> const &get_polygons() const {
		return polygons;
	}

	bool sync();

	NavRegion();
	~NavRegion();

private:
	void update_polygons();
};

#endif // NAV_REGION_H
