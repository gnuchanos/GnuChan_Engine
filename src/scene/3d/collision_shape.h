/**************************************************************************/
/*  collision_shape.h                                                     */
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

#ifndef COLLISION_SHAPE_H
#define COLLISION_SHAPE_H

#include "scene/3d/spatial.h"
#include "scene/resources/shape.h"
class CollisionObject;
class CollisionShape : public Spatial {
	GDCLASS(CollisionShape, Spatial);
	OBJ_CATEGORY("3D Physics Nodes");

	Ref<Shape> shape;

	uint32_t owner_id;
	CollisionObject *parent;

	void resource_changed(RES res);
	bool disabled;

protected:
	void _update_in_shape_owner(bool p_xform_only = false);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void make_convex_from_brothers();

	void set_shape(const Ref<Shape> &p_shape);
	Ref<Shape> get_shape() const;

	void set_disabled(bool p_disabled);
	bool is_disabled() const;

	String get_configuration_warning() const;

#ifdef TOOLS_ENABLED
	virtual AABB get_fallback_gizmo_aabb() const;
#endif

	CollisionShape();
	~CollisionShape();
};

#endif // COLLISION_SHAPE_H
