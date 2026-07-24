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

#include "register_types.h"

#include "editor/editor_node.h"
#include "editor_scene_importer_fbx.h"

#ifdef TOOLS_ENABLED
static void _editor_init() {
	Ref<EditorSceneImporterFBX> import_fbx;
	import_fbx.instance();
	ResourceImporterScene::get_singleton()->add_importer(import_fbx);
}
#endif

void register_fbx_types() {
#ifdef TOOLS_ENABLED
	ClassDB::APIType prev_api = ClassDB::get_current_api();
	ClassDB::set_current_api(ClassDB::API_EDITOR);

	ClassDB::register_class<EditorSceneImporterFBX>();

	ClassDB::set_current_api(prev_api);

	EditorNode::add_init_callback(_editor_init);
#endif
}

void unregister_fbx_types() {
}
