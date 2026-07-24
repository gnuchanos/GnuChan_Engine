/**************************************************************************/
/*  lightmap_raycaster.h                                                  */
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

#include "core/object.h"
#include "scene/3d/lightmapper.h"
#include "scene/resources/mesh.h"

#include <embree3/rtcore.h>

class LightmapRaycasterEmbree : public LightmapRaycaster {
	GDCLASS(LightmapRaycasterEmbree, LightmapRaycaster);

private:
	struct AlphaTextureData {
		Vector<uint8_t> data;
		Vector2i size;

		uint8_t sample(float u, float v) const;
	};

	RTCDevice embree_device;
	RTCScene embree_scene;

	static void filter_function(const struct RTCFilterFunctionNArguments *p_args);

	Map<unsigned int, AlphaTextureData> alpha_textures;
	Set<int> filter_meshes;

public:
	virtual bool intersect(Ray &p_ray);

	virtual void intersect(Vector<Ray> &r_rays);

	virtual void add_mesh(const Vector<Vector3> &p_vertices, const Vector<Vector3> &p_normals, const Vector<Vector2> &p_uv2s, unsigned int p_id);
	virtual void set_mesh_alpha_texture(Ref<Image> p_alpha_texture, unsigned int p_id);
	virtual void commit();

	virtual void set_mesh_filter(const Set<int> &p_mesh_ids);
	virtual void clear_mesh_filter();

	static LightmapRaycaster *create_embree_raycaster();
	static void make_default_raycaster();

	LightmapRaycasterEmbree();
	~LightmapRaycasterEmbree();
};
