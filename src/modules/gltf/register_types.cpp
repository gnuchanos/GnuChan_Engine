/**************************************************************************/
/*  register_types.cpp                                                    */
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

#ifndef _3D_DISABLED

#include "register_types.h"

#include "extensions/gltf_document_extension.h"
#include "extensions/gltf_spec_gloss.h"
#include "extensions/physics/gltf_document_extension_physics.h"
#include "gltf_state.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "editor_scene_exporter_gltf_plugin.h"
#include "editor_scene_importer_gltf.h"
#endif

#ifdef TOOLS_ENABLED
static void _editor_init() {
	Ref<EditorSceneImporterGLTF> import_gltf;
	import_gltf.instance();
	ResourceImporterScene::get_singleton()->add_importer(import_gltf);
}
#endif

#define GLTF_REGISTER_DOCUMENT_EXTENSION(m_doc_ext_class) \
	Ref<m_doc_ext_class> extension_##m_doc_ext_class;     \
	extension_##m_doc_ext_class.instance();               \
	GLTFDocument::register_gltf_document_extension(extension_##m_doc_ext_class);

void register_gltf_types() {
#ifdef TOOLS_ENABLED
	ClassDB::APIType prev_api = ClassDB::get_current_api();
	ClassDB::set_current_api(ClassDB::API_EDITOR);
	ClassDB::register_class<EditorSceneImporterGLTF>();
	ClassDB::register_class<GLTFMesh>();
	EditorPlugins::add_by_type<SceneExporterGLTFPlugin>();
	ClassDB::set_current_api(prev_api);
	EditorNode::add_init_callback(_editor_init);
#endif

	ClassDB::register_class<GLTFSpecGloss>();
	ClassDB::register_class<GLTFNode>();
	ClassDB::register_class<GLTFAnimation>();
	ClassDB::register_class<GLTFBufferView>();
	ClassDB::register_class<GLTFAccessor>();
	ClassDB::register_class<GLTFCollider>();
	ClassDB::register_class<GLTFTexture>();
	ClassDB::register_class<GLTFTextureSampler>();
	ClassDB::register_class<GLTFSkeleton>();
	ClassDB::register_class<GLTFSkin>();
	ClassDB::register_class<GLTFCamera>();
	ClassDB::register_class<GLTFLight>();
	ClassDB::register_class<GLTFPhysicsBody>();
	ClassDB::register_class<GLTFState>();
	ClassDB::register_class<GLTFDocument>();
	ClassDB::register_class<GLTFDocumentExtension>();
	ClassDB::register_class<PackedSceneGLTF>();
	GLTF_REGISTER_DOCUMENT_EXTENSION(GLTFDocumentExtensionPhysics);
}

void unregister_gltf_types() {
	GLTFDocument::unregister_all_gltf_document_extensions();
}

#endif // _3D_DISABLED
