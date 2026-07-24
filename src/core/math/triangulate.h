/**************************************************************************/
/*  triangulate.h                                                         */
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

#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "core/math/vector2.h"

/*
http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
*/

class Triangulate {
public:
	// triangulate a contour/polygon, places results in STL vector
	// as series of triangles.
	static bool triangulate(const Vector<Vector2> &contour, Vector<int> &result);

	// compute area of a contour/polygon
	static real_t get_area(const Vector<Vector2> &contour);

	// decide if point Px/Py is inside triangle defined by
	// (Ax,Ay) (Bx,By) (Cx,Cy)
	static bool is_inside_triangle(real_t Ax, real_t Ay,
			real_t Bx, real_t By,
			real_t Cx, real_t Cy,
			real_t Px, real_t Py,
			bool include_edges);

private:
	static bool snip(const Vector<Vector2> &p_contour, int u, int v, int w, int n, const Vector<int> &V, bool relaxed);
};

#endif // TRIANGULATE_H
