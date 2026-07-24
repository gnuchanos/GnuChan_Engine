/**************************************************************************/
/*  gltf_document_extension.cpp                                           */
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

#include "gltf_document_extension.h"

#include "../gltf_document.h"

void GLTFDocumentExtension::_bind_methods() {
	// Import process.
	BIND_VMETHOD(MethodInfo(Variant::INT, "_import_preflight", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::POOL_STRING_ARRAY, "extensions")));
	BIND_VMETHOD(MethodInfo(Variant::ARRAY, "_get_supported_extensions"));
	BIND_VMETHOD(MethodInfo(Variant::INT, "_parse_node_extensions", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "gltf_node", PROPERTY_HINT_RESOURCE_TYPE, "GLTFNode"), PropertyInfo(Variant::DICTIONARY, "extensions")));
	BIND_VMETHOD(MethodInfo(Variant::OBJECT, "_generate_scene_node", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "gltf_node", PROPERTY_HINT_RESOURCE_TYPE, "GLTFNode"), PropertyInfo(Variant::OBJECT, "scene_parent", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	BIND_VMETHOD(MethodInfo(Variant::INT, "_import_post_parse", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState")));
	BIND_VMETHOD(MethodInfo(Variant::INT, "_import_node", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "gltf_node", PROPERTY_HINT_RESOURCE_TYPE, "GLTFNode"), PropertyInfo(Variant::DICTIONARY, "json"), PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	BIND_VMETHOD(MethodInfo(Variant::INT, "_import_post", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "root", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	// Export process.
	BIND_VMETHOD(MethodInfo(Variant::INT, "_export_preflight", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "root", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	BIND_VMETHOD(MethodInfo("_convert_scene_node", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "gltf_node", PROPERTY_HINT_RESOURCE_TYPE, "GLTFNode"), PropertyInfo(Variant::OBJECT, "scene_node", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	BIND_VMETHOD(MethodInfo(Variant::INT, "_export_node", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState"), PropertyInfo(Variant::OBJECT, "gltf_node", PROPERTY_HINT_RESOURCE_TYPE, "GLTFNode"), PropertyInfo(Variant::DICTIONARY, "json"), PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	BIND_VMETHOD(MethodInfo(Variant::INT, "_export_post", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "GLTFState")));
}

// Import process.
Error GLTFDocumentExtension::import_preflight(Ref<GLTFState> p_state, Vector<String> p_extensions) {
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_import_preflight", p_state, p_extensions);
	return Error(err);
}

Vector<String> GLTFDocumentExtension::get_supported_extensions() {
	Vector<String> ret;
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return ret;
	}
	si->call("_get_supported_extensions");
	return ret;
}

Error GLTFDocumentExtension::parse_node_extensions(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Dictionary &p_extensions) {
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_gltf_node.is_null(), ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_parse_node_extensions", p_state, p_gltf_node, p_extensions);
	return Error(err);
}

Spatial *GLTFDocumentExtension::generate_scene_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Node *p_scene_parent) {
	ERR_FAIL_COND_V(p_state.is_null(), nullptr);
	ERR_FAIL_COND_V(p_gltf_node.is_null(), nullptr);
	ERR_FAIL_NULL_V(p_scene_parent, nullptr);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return nullptr;
	}
	Variant ret = si->call("_generate_scene_node", p_state, p_gltf_node, p_scene_parent);
	Spatial *ret_node = cast_to<Spatial>(ret.operator Object *());
	return ret_node;
}

Error GLTFDocumentExtension::import_post_parse(Ref<GLTFState> p_state) {
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_import_post_parse", p_state);
	return Error(err);
}

Error GLTFDocumentExtension::import_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Dictionary &r_dict, Node *p_node) {
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_gltf_node.is_null(), ERR_INVALID_PARAMETER);
	ERR_FAIL_NULL_V(p_node, ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_import_node", p_state, p_gltf_node, r_dict, p_node);
	return Error(err);
}

Error GLTFDocumentExtension::import_post(Ref<GLTFState> p_state, Node *p_root) {
	ERR_FAIL_NULL_V(p_root, ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_import_post", p_state, p_root);
	return Error(err);
}

// Export process.
Error GLTFDocumentExtension::export_preflight(Ref<GLTFState> p_state, Node *p_root) {
	ERR_FAIL_NULL_V(p_root, ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_export_preflight", p_root);
	return Error(err);
}

void GLTFDocumentExtension::convert_scene_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Node *p_scene_node) {
	ERR_FAIL_COND(p_state.is_null());
	ERR_FAIL_COND(p_gltf_node.is_null());
	ERR_FAIL_NULL(p_scene_node);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return;
	}
	si->call("_convert_scene_node", p_state, p_gltf_node, p_scene_node);
}

Error GLTFDocumentExtension::export_node(Ref<GLTFState> p_state, Ref<GLTFNode> p_gltf_node, Dictionary &r_dict, Node *p_node) {
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_gltf_node.is_null(), ERR_INVALID_PARAMETER);
	ERR_FAIL_NULL_V(p_node, ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_export_node", p_state, p_gltf_node, r_dict, p_node);
	return Error(err);
}

Error GLTFDocumentExtension::export_post(Ref<GLTFState> p_state) {
	ERR_FAIL_COND_V(p_state.is_null(), ERR_INVALID_PARAMETER);
	ScriptInstance *si = get_script_instance();
	if (!si) {
		return Error::OK;
	}
	int err = si->call("_export_post", p_state);
	return Error(err);
}
