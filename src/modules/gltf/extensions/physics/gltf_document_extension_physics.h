/**************************************************************************/
/*  gltf_document_extension_physics.h                                     */
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

#ifndef GLTF_DOCUMENT_EXTENSION_PHYSICS_H
#define GLTF_DOCUMENT_EXTENSION_PHYSICS_H

#include "../gltf_document_extension.h"

#include "gltf_collider.h"
#include "gltf_physics_body.h"

class GLTFDocumentExtensionPhysics : public GLTFDocumentExtension {
	GDCLASS(GLTFDocumentExtensionPhysics, GLTFDocumentExtension);

public:
	// Import process.
	Error import_preflight(Ref<GLTFState> p_state, Vector<String> p_extensions);
	Vector<String> get_supported_extensions();
	Error parse_node_extensions(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Dictionary &p_extensions);
	Spatial *generate_scene_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Node *p_scene_parent);
	// Export process.
	void convert_scene_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Node *p_scene_node);
	Error export_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Dictionary &r_node_json, Node *p_scene_node);
};

#endif // GLTF_DOCUMENT_EXTENSION_PHYSICS_H
