/**************************************************************************/
/*  navigation_mesh_generator.h                                           */
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

#ifndef NAVIGATION_MESH_GENERATOR_H
#define NAVIGATION_MESH_GENERATOR_H

#ifndef _3D_DISABLED

#include "scene/3d/navigation_mesh_instance.h"

#include <Recast.h>

#ifdef TOOLS_ENABLED
struct EditorProgress;
#endif

class NavigationMeshGenerator : public Object {
	GDCLASS(NavigationMeshGenerator, Object);

	static NavigationMeshGenerator *singleton;

protected:
	static void _bind_methods();

	static void _add_vertex(const Vector3 &p_vec3, Vector<float> &p_vertices);
	static void _add_mesh(const Ref<Mesh> &p_mesh, const Transform &p_xform, Vector<float> &p_vertices, Vector<int> &p_indices);
	static void _add_mesh_array(const Array &p_array, const Transform &p_xform, Vector<float> &p_vertices, Vector<int> &p_indices);
	static void _add_faces(const PoolVector3Array &p_faces, const Transform &p_xform, Vector<float> &p_vertices, Vector<int> &p_indices);
	static void _parse_geometry(const Transform &p_navmesh_xform, Node *p_node, Vector<float> &p_vertices, Vector<int> &p_indices, int p_generate_from, uint32_t p_collision_mask, bool p_recurse_children);

	static void _convert_detail_mesh_to_native_navigation_mesh(const rcPolyMeshDetail *p_detail_mesh, Ref<NavigationMesh> p_nav_mesh);
	static void _build_recast_navigation_mesh(
			Ref<NavigationMesh> p_nav_mesh,
#ifdef TOOLS_ENABLED
			EditorProgress *ep,
#endif
			rcHeightfield *hf,
			rcCompactHeightfield *chf,
			rcContourSet *cset,
			rcPolyMesh *poly_mesh,
			rcPolyMeshDetail *detail_mesh,
			Vector<float> &vertices,
			Vector<int> &indices);

public:
	static NavigationMeshGenerator *get_singleton();

	NavigationMeshGenerator();
	~NavigationMeshGenerator();

	void bake(Ref<NavigationMesh> p_nav_mesh, Node *p_node);
	void clear(Ref<NavigationMesh> p_nav_mesh);
};

#endif

#endif // NAVIGATION_MESH_GENERATOR_H
