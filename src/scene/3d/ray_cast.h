/**************************************************************************/
/*  ray_cast.h                                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHANIDE ENGINE                          */
/*                        https://github.com/gnuchanos                    */
/**************************************************************************/
/*  RayCast 3D:                                                          */
/*    - Query: is_colliding / get_collision_point / get_collision_normal */
/*    - Target info: body_name() / body_group() / get_collider()         */
/*    - Ignoring: skip(node) / skip_group(group)                         */
/*    - Settings: enabled / cast_to / length                             */
/*    - Editor debug visuals (used by spatial_editor_gizmos)             */
/*                                                                        */
/*  Permission is hereby granted, free of charge, to any person obtaining */
/*  a copy of this software and associated documentation files (the       */
/*  "Software"), to deal in the Software without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,   */
/*  distribute, sublicense, and/or sell copies of the Software, and to    */
/*  permit persons to whom the Software is furnished to do so, subject to */
/*  the following conditions:                                             */
/*                                                                        */
/*  The above copyright notice and this permission notice shall be        */
/*  included in all copies or substantial portions of the Software.       */
/*                                                                        */
/*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/*  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/*  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/*  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/*  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/**************************************************************************/

#ifndef RAY_CAST_H
#define RAY_CAST_H

#include "scene/3d/spatial.h"

class RayCast : public Spatial {
	GDCLASS(RayCast, Spatial);

	bool enabled;
	bool collided;
	ObjectID against;
	Vector3 collision_point;
	Vector3 collision_normal;

	Vector3 cast_to;
	Set<RID> exclude;

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

	bool collide_with_bodies;
	bool collide_with_areas;

	void _update_raycast_state();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	/* --- Query --- */
	bool is_colliding() const;
	Object *get_collider() const;
	Vector3 get_collision_point() const;
	Vector3 get_collision_normal() const;

	/* Quick info about the hit target (GnuChan addition) */
	String get_body_name() const;
	String get_body_group() const;

	/* --- Ignoring (GnuChan addition) --- */
	/* Skip the passed node and every physics body in its subtree. */
	void skip(const Object *p_object);

	/* Skip every physics body that is a member of the given group. */
	void skip_group(const StringName &p_group);

	/* --- Settings --- */
	void set_enabled(bool p_enabled);
	bool is_enabled() const;

	void set_cast_to(const Vector3 &p_point);
	Vector3 get_cast_to() const;

	/* Ray length: cast_to.z = -length (for FPS use) */
	void set_length(real_t p_length);
	real_t get_length() const;

	void set_collide_with_bodies(bool p_clip);
	bool is_collide_with_bodies_enabled() const;
	void set_collide_with_areas(bool p_clip);
	bool is_collide_with_areas_enabled() const;

	void force_raycast_update();

	/* --- Editor debug visuals (used by spatial_editor_gizmos) --- */
	const Color &get_debug_shape_custom_color() const;
	void set_debug_shape_custom_color(const Color &p_color);

	const Vector<Vector3> &get_debug_shape_vertices() const;
	const Vector<Vector3> &get_debug_line_vertices() const;

	Ref<Material3D> get_debug_material();

	int get_debug_shape_thickness() const;
	void set_debug_shape_thickness(const int p_debug_thickness);

	RayCast();
};

#endif // RAY_CAST_H

