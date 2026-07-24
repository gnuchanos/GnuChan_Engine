/**************************************************************************/
/*  quick_hull.h                                                          */
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

#ifndef QUICK_HULL_H
#define QUICK_HULL_H

#include "core/list.h"
#include "core/math/aabb.h"
#include "core/math/geometry.h"
#include "core/set.h"

class QuickHull {
public:
	struct Edge {
		union {
			uint32_t vertices[2];
			uint64_t id;
		};

		bool operator<(const Edge &p_edge) const {
			return id < p_edge.id;
		}

		Edge(int p_vtx_a = 0, int p_vtx_b = 0) {
			if (p_vtx_a > p_vtx_b) {
				SWAP(p_vtx_a, p_vtx_b);
			}

			vertices[0] = p_vtx_a;
			vertices[1] = p_vtx_b;
		}
	};

	struct Face {
		Plane plane;
		uint32_t vertices[3];
		Vector<int> points_over;

		bool operator<(const Face &p_face) const {
			return points_over.size() < p_face.points_over.size();
		}
	};

private:
	struct FaceConnect {
		List<Face>::Element *left, *right;
		FaceConnect() {
			left = nullptr;
			right = nullptr;
		}
	};
	struct RetFaceConnect {
		List<Geometry::MeshData::Face>::Element *left, *right;
		RetFaceConnect() {
			left = nullptr;
			right = nullptr;
		}
	};

	static int find_or_create_output_index(int p_old_index, Vector<int> &r_out_indices) {
		for (int n = 0; n < r_out_indices.size(); n++) {
			if (r_out_indices[n] == p_old_index) {
				return n;
			}
		}
		r_out_indices.push_back(p_old_index);
		return r_out_indices.size() - 1;
	}

public:
	static uint32_t debug_stop_after;
	static bool _flag_warnings;
	static Error build(const Vector<Vector3> &p_points, Geometry::MeshData &r_mesh, real_t p_over_tolerance_epsilon = 3.0 * UNIT_EPSILON);
};

#endif // QUICK_HULL_H
