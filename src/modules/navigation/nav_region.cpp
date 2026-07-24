/**************************************************************************/
/*  nav_region.cpp                                                        */
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

#include "nav_region.h"

#include "nav_map.h"

void NavRegion::set_map(NavMap *p_map) {
	map = p_map;
	polygons_dirty = true;
	if (!map) {
		connections.clear();
	}
}

void NavRegion::set_navigation_layers(uint32_t p_navigation_layers) {
	navigation_layers = p_navigation_layers;
}

uint32_t NavRegion::get_navigation_layers() const {
	return navigation_layers;
}

void NavRegion::set_transform(Transform p_transform) {
	transform = p_transform;
	polygons_dirty = true;
}

void NavRegion::set_mesh(Ref<NavigationMesh> p_mesh) {
	mesh = p_mesh;
	polygons_dirty = true;
}

int NavRegion::get_connections_count() const {
	if (!map) {
		return 0;
	}
	return connections.size();
}

Vector3 NavRegion::get_connection_pathway_start(int p_connection_id) const {
	ERR_FAIL_COND_V(!map, Vector3());
	ERR_FAIL_INDEX_V(p_connection_id, connections.size(), Vector3());
	return connections[p_connection_id].pathway_start;
}

Vector3 NavRegion::get_connection_pathway_end(int p_connection_id) const {
	ERR_FAIL_COND_V(!map, Vector3());
	ERR_FAIL_INDEX_V(p_connection_id, connections.size(), Vector3());
	return connections[p_connection_id].pathway_end;
}

bool NavRegion::sync() {
	bool something_changed = polygons_dirty /* || something_dirty? */;

	update_polygons();

	return something_changed;
}

void NavRegion::update_polygons() {
	if (!polygons_dirty) {
		return;
	}
	polygons.clear();
	polygons_dirty = false;

	if (map == nullptr) {
		return;
	}

	if (mesh.is_null()) {
		return;
	}

	PoolVector<Vector3> vertices = mesh->get_vertices();
	int len = vertices.size();
	if (len == 0) {
		return;
	}

	PoolVector<Vector3>::Read vertices_r = vertices.read();

	polygons.resize(mesh->get_polygon_count());

	// Build
	for (size_t i(0); i < polygons.size(); i++) {
		gd::Polygon &p = polygons[i];
		p.owner = this;

		Vector<int> mesh_poly = mesh->get_polygon(i);
		const int *indices = mesh_poly.ptr();
		bool valid(true);
		p.points.resize(mesh_poly.size());
		p.edges.resize(mesh_poly.size());

		Vector3 center;
		float sum(0);

		for (int j(0); j < mesh_poly.size(); j++) {
			int idx = indices[j];
			if (idx < 0 || idx >= len) {
				valid = false;
				break;
			}

			Vector3 point_position = transform.xform(vertices_r[idx]);
			p.points[j].pos = point_position;
			p.points[j].key = map->get_point_key(point_position);

			center += point_position; // Composing the center of the polygon

			if (j >= 2) {
				Vector3 epa = transform.xform(vertices_r[indices[j - 2]]);
				Vector3 epb = transform.xform(vertices_r[indices[j - 1]]);

				sum += map->get_up().dot((epb - epa).cross(point_position - epa));
			}
		}

		if (!valid) {
			ERR_BREAK_MSG(!valid, "The navigation mesh set in this region is not valid!");
		}

		p.clockwise = sum > 0;
		if (mesh_poly.size() != 0) {
			p.center = center / float(mesh_poly.size());
		}
	}
}

NavRegion::NavRegion() {}

NavRegion::~NavRegion() {}
