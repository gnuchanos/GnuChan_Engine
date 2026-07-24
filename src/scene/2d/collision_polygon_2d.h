/**************************************************************************/
/*  collision_polygon_2d.h                                                */
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

#ifndef COLLISION_POLYGON_2D_H
#define COLLISION_POLYGON_2D_H

#include "scene/2d/node_2d.h"
#include "scene/resources/shape_2d.h"

class CollisionObject2D;

class CollisionPolygon2D : public Node2D {
	GDCLASS(CollisionPolygon2D, Node2D);

public:
	enum BuildMode {
		BUILD_SOLIDS,
		BUILD_SEGMENTS,
	};

protected:
	Rect2 aabb;
	BuildMode build_mode;
	Vector<Point2> polygon;
	uint32_t owner_id;
	CollisionObject2D *parent;
	bool disabled;
	bool one_way_collision;
	float one_way_collision_margin;

	Vector<Vector<Vector2>> _decompose_in_convex();

	void _build_polygon();

	void _update_in_shape_owner(bool p_xform_only = false);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const;
#endif

	void set_build_mode(BuildMode p_mode);
	BuildMode get_build_mode() const;

	void set_polygon(const Vector<Point2> &p_polygon);
	Vector<Point2> get_polygon() const;

	virtual String get_configuration_warning() const;

	void set_disabled(bool p_disabled);
	bool is_disabled() const;

	void set_one_way_collision(bool p_enable);
	bool is_one_way_collision_enabled() const;

	void set_one_way_collision_margin(float p_margin);
	float get_one_way_collision_margin() const;

	CollisionPolygon2D();
};

VARIANT_ENUM_CAST(CollisionPolygon2D::BuildMode);

#endif // COLLISION_POLYGON_2D_H
