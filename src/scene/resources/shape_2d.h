/**************************************************************************/
/*  shape_2d.h                                                            */
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

#ifndef SHAPE_2D_H
#define SHAPE_2D_H

#include "core/resource.h"

class Shape2D : public Resource {
	GDCLASS(Shape2D, Resource);
	OBJ_SAVE_TYPE(Shape2D);

	RID shape;
	real_t custom_bias;

protected:
	static void _bind_methods();
	Shape2D(const RID &p_rid);

public:
	virtual bool _edit_is_selected_on_click(const Point2 &p_point, double p_tolerance) const { return get_rect().has_point(p_point); }

	void set_custom_solver_bias(real_t p_bias);
	real_t get_custom_solver_bias() const;

	bool collide_with_motion(const Transform2D &p_local_xform, const Vector2 &p_local_motion, const Ref<Shape2D> &p_shape, const Transform2D &p_shape_xform, const Vector2 &p_shape_motion);
	bool collide(const Transform2D &p_local_xform, const Ref<Shape2D> &p_shape, const Transform2D &p_shape_xform);

	Array collide_with_motion_and_get_contacts(const Transform2D &p_local_xform, const Vector2 &p_local_motion, const Ref<Shape2D> &p_shape, const Transform2D &p_shape_xform, const Vector2 &p_shape_motion);
	Array collide_and_get_contacts(const Transform2D &p_local_xform, const Ref<Shape2D> &p_shape, const Transform2D &p_shape_xform);

	virtual void draw(const RID &p_to_rid, const Color &p_color) {}
	virtual Rect2 get_rect() const { return Rect2(); }
	/// Returns the radius of a sphere that fully enclose this shape
	virtual real_t get_enclosing_radius() const = 0;
	virtual RID get_rid() const;

	bool is_collision_outline_enabled();

	~Shape2D();
};

#endif // SHAPE_2D_H
