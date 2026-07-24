/**************************************************************************/
/*  packed_scene_gltf.h                                                   */
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

#ifndef PACKED_SCENE_GLTF_H
#define PACKED_SCENE_GLTF_H

#ifdef TOOLS_ENABLED

#include "scene/main/node.h"
#include "scene/resources/packed_scene.h"

#include "gltf_state.h"

class PackedSceneGLTF : public PackedScene {
	GDCLASS(PackedSceneGLTF, PackedScene);

protected:
	static void _bind_methods();

public:
	virtual void save_scene(Node *p_node, const String &p_path, const String &p_src_path,
			uint32_t p_flags, int p_bake_fps,
			List<String> *r_missing_deps, Error *r_err = nullptr);
	virtual void _build_parent_hierachy(Ref<GLTFState> state);
	virtual Error export_gltf(Node *p_root, String p_path, int32_t p_flags = 0,
			real_t p_bake_fps = 1000.0f);
	virtual Node *import_scene(const String &p_path, uint32_t p_flags,
			int p_bake_fps, uint32_t p_compress_flags,
			List<String> *r_missing_deps,
			Error *r_err,
			Ref<GLTFState> r_state);
	virtual Node *import_gltf_scene(const String &p_path, uint32_t p_flags, float p_bake_fps, uint32_t p_compress_flags, Ref<GLTFState> r_state = Ref<GLTFState>());
	virtual void pack_gltf(String p_path, int32_t p_flags = 0,
			real_t p_bake_fps = 1000.0f, uint32_t p_compress_flags = Mesh::ARRAY_COMPRESS_DEFAULT, Ref<GLTFState> r_state = Ref<GLTFState>());
};

#endif // TOOLS_ENABLED

#endif // PACKED_SCENE_GLTF_H
