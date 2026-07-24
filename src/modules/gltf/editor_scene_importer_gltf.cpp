/**************************************************************************/
/*  editor_scene_importer_gltf.cpp                                        */
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

#ifdef TOOLS_ENABLED

#include "editor_scene_importer_gltf.h"
#include "scene/resources/animation.h"

#include "gltf_state.h"
#include "packed_scene_gltf.h"

uint32_t EditorSceneImporterGLTF::get_import_flags() const {
	return ImportFlags::IMPORT_SCENE | ImportFlags::IMPORT_ANIMATION;
}

void EditorSceneImporterGLTF::get_extensions(List<String> *r_extensions) const {
	r_extensions->push_back("gltf");
	r_extensions->push_back("glb");
}

Node *EditorSceneImporterGLTF::import_scene(const String &p_path,
		uint32_t p_flags, int p_bake_fps, uint32_t p_compress_flags,
		List<String> *r_missing_deps,
		Error *r_err) {
	Ref<PackedSceneGLTF> importer;
	importer.instance();
	return importer->import_scene(p_path, p_flags, p_bake_fps, p_compress_flags, r_missing_deps, r_err, Ref<GLTFState>());
}

Ref<Animation> EditorSceneImporterGLTF::import_animation(const String &p_path,
		uint32_t p_flags,
		int p_bake_fps) {
	return Ref<Animation>();
}

#endif // TOOLS_ENABLED
