/**************************************************************************/
/*  immediate_geometry.h                                                  */
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

#ifndef IMMEDIATE_GEOMETRY_H
#define IMMEDIATE_GEOMETRY_H

#include "scene/3d/visual_instance.h"
#include "scene/resources/mesh.h"

class ImmediateGeometry : public GeometryInstance {
	GDCLASS(ImmediateGeometry, GeometryInstance);

	RID im;
	//a list of textures drawn need to be kept, to avoid references
	// in VisualServer from becoming invalid if the texture is no longer used
	List<Ref<Texture>> cached_textures;
	bool empty;
	AABB aabb;

protected:
	static void _bind_methods();

public:
	void begin(Mesh::PrimitiveType p_primitive, const Ref<Texture> &p_texture = Ref<Texture>());
	void set_normal(const Vector3 &p_normal);
	void set_tangent(const Plane &p_tangent);
	void set_color(const Color &p_color);
	void set_uv(const Vector2 &p_uv);
	void set_uv2(const Vector2 &p_uv2);

	void add_vertex(const Vector3 &p_vertex);

	void end();
	void clear();

	void add_sphere(int p_lats, int p_lons, float p_radius, bool p_add_uv = true);

	virtual AABB get_aabb() const;
	virtual PoolVector<Face3> get_faces(uint32_t p_usage_flags) const;

	ImmediateGeometry();
	~ImmediateGeometry();
};

#endif // IMMEDIATE_GEOMETRY_H
