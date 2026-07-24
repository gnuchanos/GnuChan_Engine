/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"
#include "scene/resources/mesh.h"
#include "thirdparty/vhacd/public/VHACD.h"

static Vector<PoolVector<Vector3>> convex_decompose(const real_t *p_vertices, int p_vertex_count, const uint32_t *p_triangles, int p_triangle_count, int p_max_convex_hulls = -1, Vector<PoolVector<uint32_t>> *r_convex_indices = nullptr) {
	VHACD::IVHACD::Parameters params;
	if (p_max_convex_hulls > 0) {
		params.m_maxConvexHulls = p_max_convex_hulls;
	}

	VHACD::IVHACD *decomposer = VHACD::CreateVHACD();
	decomposer->Compute(p_vertices, p_vertex_count, p_triangles, p_triangle_count, params);

	int hull_count = decomposer->GetNConvexHulls();

	Vector<PoolVector<Vector3>> ret;
	ret.resize(hull_count);

	if (r_convex_indices) {
		r_convex_indices->resize(hull_count);
	}

	for (int i = 0; i < hull_count; i++) {
		VHACD::IVHACD::ConvexHull hull;
		decomposer->GetConvexHull(i, hull);

		PoolVector<Vector3> &points = ret.write[i];
		points.resize(hull.m_nPoints);

		PoolVector<Vector3>::Write w = points.write();
		for (uint32_t j = 0; j < hull.m_nPoints; ++j) {
			for (int k = 0; k < 3; k++) {
				w[j][k] = hull.m_points[j * 3 + k];
			}
		}

		if (r_convex_indices) {
			PoolVector<uint32_t> &indices = r_convex_indices->write[i];
			indices.resize(hull.m_nTriangles * 3);

			memcpy(indices.write().ptr(), hull.m_triangles, hull.m_nTriangles * 3 * sizeof(uint32_t));
		}
	}

	decomposer->Clean();
	decomposer->Release();

	return ret;
}

void register_vhacd_types() {
	Mesh::convex_decomposition_function = convex_decompose;
}

void unregister_vhacd_types() {
	Mesh::convex_decomposition_function = nullptr;
}
