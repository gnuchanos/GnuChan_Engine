/**************************************************************************/
/*  ray_cast.h                                                            */
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

#ifndef RAY_CAST_H
#define RAY_CAST_H

#include "scene/3d/spatial.h"

class RayCast : public Spatial {
	GDCLASS(RayCast, Spatial);

	bool enabled;
	bool collided;
	ObjectID against;
	int against_shape;
	Vector3 collision_point;
	Vector3 collision_normal;

	Vector3 cast_to;
	Set<RID> exclude;

	uint32_t collision_mask;
	bool exclude_parent_body;

	Node *debug_shape;
	Ref<Material> debug_material;
	Color debug_shape_custom_color = Color(0.0, 0.0, 0.0);
	int debug_shape_thickness = 2;
	Vector<Vector3> debug_shape_vertices;
	Vector<Vector3> debug_line_vertices;

	void _create_debug_shape();
	void _update_debug_shape();
	void _update_debug_shape_material(bool p_check_collision = false);
	void _update_debug_shape_vertices();
	void _clear_debug_shape();

	bool collide_with_areas;
	bool collide_with_bodies;

protected:
	void _notification(int p_what);
	void _update_raycast_state();
	static void _bind_methods();

public:
	void set_collide_with_areas(bool p_clip);
	bool is_collide_with_areas_enabled() const;

	void set_collide_with_bodies(bool p_clip);
	bool is_collide_with_bodies_enabled() const;

	void set_enabled(bool p_enabled);
	bool is_enabled() const;

	void set_cast_to(const Vector3 &p_point);
	Vector3 get_cast_to() const;

	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() const;

	void set_collision_mask_bit(int p_bit, bool p_value);
	bool get_collision_mask_bit(int p_bit) const;

	void set_exclude_parent_body(bool p_exclude_parent_body);
	bool get_exclude_parent_body() const;

	const Color &get_debug_shape_custom_color() const;
	void set_debug_shape_custom_color(const Color &p_color);

	const Vector<Vector3> &get_debug_shape_vertices() const;
	const Vector<Vector3> &get_debug_line_vertices() const;

	Ref<Material3D> get_debug_material();

	int get_debug_shape_thickness() const;
	void set_debug_shape_thickness(const int p_debug_thickness);

	void force_raycast_update();
	bool is_colliding() const;
	Object *get_collider() const;
	int get_collider_shape() const;
	Vector3 get_collision_point() const;
	Vector3 get_collision_normal() const;

	void add_exception_rid(const RID &p_rid);
	void add_exception(const Object *p_object);
	void remove_exception_rid(const RID &p_rid);
	void remove_exception(const Object *p_object);
	void clear_exceptions();

	RayCast();
};

#endif // RAY_CAST_H
