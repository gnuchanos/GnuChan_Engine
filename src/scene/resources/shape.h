/**************************************************************************/
/*  shape.h                                                               */
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

#ifndef SHAPE_H
#define SHAPE_H

#include "core/resource.h"
class ArrayMesh;

class Shape : public Resource {
	GDCLASS(Shape, Resource);
	OBJ_SAVE_TYPE(Shape);
	RES_BASE_EXTENSION("shape");
	RID shape;
	real_t margin;

	Ref<ArrayMesh> debug_mesh_cache;

protected:
	static void _bind_methods();

	_FORCE_INLINE_ RID get_shape() const { return shape; }
	Shape(RID p_shape);

	virtual void _update_shape();

public:
	virtual RID get_rid() const { return shape; }

	Ref<ArrayMesh> get_debug_mesh();
	virtual Vector<Vector3> get_debug_mesh_lines() = 0; // { return Vector<Vector3>(); }
	/// Returns the radius of a sphere that fully enclose this shape
	virtual real_t get_enclosing_radius() const = 0;

	void add_vertices_to_array(PoolVector<Vector3> &array, const Transform &p_xform);

	real_t get_margin() const;
	void set_margin(real_t p_margin);

	Shape();
	~Shape();
};

#endif // SHAPE_H
