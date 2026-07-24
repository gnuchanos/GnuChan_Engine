/**************************************************************************/
/*  circle_shape_2d.cpp                                                   */
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

#include "circle_shape_2d.h"

#include "servers/physics_2d_server.h"
#include "servers/visual_server.h"

bool CircleShape2D::_edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const {
	return p_point.length() < get_radius() + p_tolerance;
}

void CircleShape2D::_update_shape() {
	Physics2DServer::get_singleton()->shape_set_data(get_rid(), radius);
	emit_changed();
}

void CircleShape2D::set_radius(real_t p_radius) {
	radius = p_radius;
	_update_shape();
}

real_t CircleShape2D::get_radius() const {
	return radius;
}

void CircleShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &CircleShape2D::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &CircleShape2D::get_radius);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius", PROPERTY_HINT_RANGE, "0.01,1024,0.01,or_greater"), "set_radius", "get_radius");
}

Rect2 CircleShape2D::get_rect() const {
	Rect2 rect;
	rect.position = -Point2(get_radius(), get_radius());
	rect.size = Point2(get_radius(), get_radius()) * 2.0;
	return rect;
}

real_t CircleShape2D::get_enclosing_radius() const {
	return radius;
}

void CircleShape2D::draw(const RID &p_to_rid, const Color &p_color) {
	Vector<Vector2> points;
	for (int i = 0; i < 24; i++) {
		points.push_back(Vector2(Math::cos(i * Math_PI * 2 / 24.0), Math::sin(i * Math_PI * 2 / 24.0)) * get_radius());
	}

	Vector<Color> col;
	col.push_back(p_color);
	VisualServer::get_singleton()->canvas_item_add_polygon(p_to_rid, points, col);
	if (is_collision_outline_enabled()) {
		VisualServer::get_singleton()->canvas_item_add_polyline(p_to_rid, points, col, 1.0, true);
		// Draw the last segment as it's not drawn by `canvas_item_add_polyline()`.
		VisualServer::get_singleton()->canvas_item_add_line(p_to_rid, points[points.size() - 1], points[0], p_color, 1.0, true);
	}
}

CircleShape2D::CircleShape2D() :
		Shape2D(Physics2DServer::get_singleton()->circle_shape_create()) {
	radius = 10;
	_update_shape();
}
