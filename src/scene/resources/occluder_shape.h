/**************************************************************************/
/*  occluder_shape.h                                                      */
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

#ifndef OCCLUDER_SHAPE_H
#define OCCLUDER_SHAPE_H

#include "core/math/plane.h"
#include "core/resource.h"
#include "core/vector.h"

class OccluderShape : public Resource {
	GDCLASS(OccluderShape, Resource);
	OBJ_SAVE_TYPE(OccluderShape);
	RES_BASE_EXTENSION("occ");
	RID _shape;

protected:
	static void _bind_methods();

	RID get_shape() const { return _shape; }
	OccluderShape();

public:
	virtual RID get_rid() const { return _shape; }
	~OccluderShape();

	virtual Transform center_node(const Transform &p_global_xform, const Transform &p_parent_xform, real_t p_snap) = 0;

#ifdef TOOLS_ENABLED
	// for editor gizmo
	virtual AABB get_fallback_gizmo_aabb() const;
	virtual bool requires_uniform_scale() const { return false; }
#endif
};

class OccluderShapeSphere : public OccluderShape {
	GDCLASS(OccluderShapeSphere, OccluderShape);

	// We bandit a plane to store position / radius
	Vector<Plane> _spheres;
	const real_t _min_radius = 0.1;

#ifdef TOOLS_ENABLED
	AABB _aabb_local;
	void _update_aabb();
#endif

protected:
	static void _bind_methods();

public:
	void set_spheres(const Vector<Plane> &p_spheres);
	Vector<Plane> get_spheres() const { return _spheres; }

	void set_sphere_position(int p_idx, const Vector3 &p_position);
	void set_sphere_radius(int p_idx, real_t p_radius);

	void update_shape_to_visual_server();
	virtual Transform center_node(const Transform &p_global_xform, const Transform &p_parent_xform, real_t p_snap);

#ifdef TOOLS_ENABLED
	virtual AABB get_fallback_gizmo_aabb() const;
	virtual bool requires_uniform_scale() const { return false; }
#endif

	OccluderShapeSphere();
};

#endif // OCCLUDER_SHAPE_H
