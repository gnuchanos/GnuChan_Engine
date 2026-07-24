/**************************************************************************/
/*  convex_polygon_shape_2d.h                                             */
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

#ifndef CONVEX_POLYGON_SHAPE_2D_H
#define CONVEX_POLYGON_SHAPE_2D_H

#include "scene/resources/shape_2d.h"

class ConvexPolygonShape2D : public Shape2D {
	GDCLASS(ConvexPolygonShape2D, Shape2D);

	Vector<Vector2> points;
	void _update_shape();

protected:
	static void _bind_methods();

public:
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const;

	void set_point_cloud(const Vector<Vector2> &p_points);
	void set_points(const Vector<Vector2> &p_points);
	Vector<Vector2> get_points() const;

	virtual void draw(const RID &p_to_rid, const Color &p_color);
	virtual Rect2 get_rect() const;
	virtual real_t get_enclosing_radius() const;

	ConvexPolygonShape2D();
};

#endif // CONVEX_POLYGON_SHAPE_2D_H
