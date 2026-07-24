/**************************************************************************/
/*  concave_polygon_shape.h                                               */
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

#ifndef CONCAVE_POLYGON_SHAPE_H
#define CONCAVE_POLYGON_SHAPE_H

#include "scene/resources/shape.h"

class ConcavePolygonShape : public Shape {
	GDCLASS(ConcavePolygonShape, Shape);

	struct DrawEdge {
		Vector3 a;
		Vector3 b;
		bool operator<(const DrawEdge &p_edge) const {
			if (a == p_edge.a) {
				return b < p_edge.b;
			} else {
				return a < p_edge.a;
			}
		}

		DrawEdge(const Vector3 &p_a = Vector3(), const Vector3 &p_b = Vector3()) {
			a = p_a;
			b = p_b;
			if (a < b) {
				SWAP(a, b);
			}
		}
	};

protected:
	static void _bind_methods();

	virtual void _update_shape();

public:
	void set_faces(const PoolVector<Vector3> &p_faces);
	PoolVector<Vector3> get_faces() const;

	Vector<Vector3> get_debug_mesh_lines();
	virtual real_t get_enclosing_radius() const;

	ConcavePolygonShape();
};

#endif // CONCAVE_POLYGON_SHAPE_H
