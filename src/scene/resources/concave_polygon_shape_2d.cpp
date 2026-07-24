/**************************************************************************/
/*  concave_polygon_shape_2d.cpp                                          */
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

#include "concave_polygon_shape_2d.h"

#include "servers/physics_2d_server.h"
#include "servers/visual_server.h"

bool ConcavePolygonShape2D::_edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const {
	PoolVector<Vector2> s = get_segments();
	int len = s.size();
	if (len == 0 || (len % 2) == 1) {
		return false;
	}

	PoolVector<Vector2>::Read r = s.read();
	for (int i = 0; i < len; i += 2) {
		Vector2 closest = Geometry::get_closest_point_to_segment_2d(p_point, &r[i]);
		if (p_point.distance_to(closest) < p_tolerance) {
			return true;
		}
	}

	return false;
}

void ConcavePolygonShape2D::set_segments(const PoolVector<Vector2> &p_segments) {
	Physics2DServer::get_singleton()->shape_set_data(get_rid(), p_segments);
	emit_changed();
}

PoolVector<Vector2> ConcavePolygonShape2D::get_segments() const {
	return Physics2DServer::get_singleton()->shape_get_data(get_rid());
}

void ConcavePolygonShape2D::draw(const RID &p_to_rid, const Color &p_color) {
	PoolVector<Vector2> s = get_segments();
	int len = s.size();
	if (len == 0 || (len % 2) == 1) {
		return;
	}

	PoolVector<Vector2>::Read r = s.read();
	for (int i = 0; i < len; i += 2) {
		VisualServer::get_singleton()->canvas_item_add_line(p_to_rid, r[i], r[i + 1], p_color, 2);
	}
}

Rect2 ConcavePolygonShape2D::get_rect() const {
	PoolVector<Vector2> s = get_segments();
	int len = s.size();
	if (len == 0) {
		return Rect2();
	}

	Rect2 rect;

	PoolVector<Vector2>::Read r = s.read();
	for (int i = 0; i < len; i++) {
		if (i == 0) {
			rect.position = r[i];
		} else {
			rect.expand_to(r[i]);
		}
	}

	return rect;
}

real_t ConcavePolygonShape2D::get_enclosing_radius() const {
	PoolVector<Vector2> data = get_segments();
	PoolVector<Vector2>::Read read = data.read();
	real_t r = 0;
	for (int i(0); i < data.size(); i++) {
		r = MAX(read[i].length_squared(), r);
	}
	return Math::sqrt(r);
}

void ConcavePolygonShape2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_segments", "segments"), &ConcavePolygonShape2D::set_segments);
	ClassDB::bind_method(D_METHOD("get_segments"), &ConcavePolygonShape2D::get_segments);

	ADD_PROPERTY(PropertyInfo(Variant::POOL_VECTOR2_ARRAY, "segments"), "set_segments", "get_segments");
}

ConcavePolygonShape2D::ConcavePolygonShape2D() :
		Shape2D(Physics2DServer::get_singleton()->concave_polygon_shape_create()) {
	PoolVector<Vector2> empty;
	set_segments(empty);
}
