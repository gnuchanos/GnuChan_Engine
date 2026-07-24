/**************************************************************************/
/*  mesh_library.h                                                        */
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

#ifndef MESH_LIBRARY_H
#define MESH_LIBRARY_H

#include "core/map.h"
#include "core/resource.h"
#include "mesh.h"
#include "scene/3d/navigation_mesh_instance.h"
#include "shape.h"

class MeshLibrary : public Resource {
	GDCLASS(MeshLibrary, Resource);
	RES_BASE_EXTENSION("meshlib");

public:
	struct ShapeData {
		Ref<Shape> shape;
		Transform local_transform;
	};
	struct Item {
		String name;
		Ref<Mesh> mesh;
		Vector<ShapeData> shapes;
		Ref<Texture> preview;
		Transform navmesh_transform;
		Transform mesh_transform;
		Ref<NavigationMesh> navmesh;
	};

	Map<int, Item> item_map;

	void _set_item_shapes(int p_item, const Array &p_shapes);
	Array _get_item_shapes(int p_item) const;

protected:
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void _bind_methods();

public:
	void create_item(int p_item);
	void set_item_name(int p_item, const String &p_name);
	void set_item_mesh(int p_item, const Ref<Mesh> &p_mesh);
	void set_item_mesh_transform(int p_item, const Transform &p_transform);
	void set_item_navmesh(int p_item, const Ref<NavigationMesh> &p_navmesh);
	void set_item_navmesh_transform(int p_item, const Transform &p_transform);
	void set_item_shapes(int p_item, const Vector<ShapeData> &p_shapes);
	void set_item_preview(int p_item, const Ref<Texture> &p_preview);
	String get_item_name(int p_item) const;
	Ref<Mesh> get_item_mesh(int p_item) const;
	Transform get_item_mesh_transform(int p_item) const;
	Ref<NavigationMesh> get_item_navmesh(int p_item) const;
	Transform get_item_navmesh_transform(int p_item) const;
	Vector<ShapeData> get_item_shapes(int p_item) const;
	Ref<Texture> get_item_preview(int p_item) const;

	void remove_item(int p_item);
	bool has_item(int p_item) const;

	void clear();

	int find_item_by_name(const String &p_name) const;

	Vector<int> get_item_list() const;
	int get_last_unused_item_id() const;

	MeshLibrary();
	~MeshLibrary();
};

#endif // MESH_LIBRARY_H
