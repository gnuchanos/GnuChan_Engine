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

#include "core/engine.h"
#include "godot_navigation_server.h"
#include "servers/navigation_server.h"

#ifndef _3D_DISABLED
#include "navigation_mesh_generator.h"
#endif

#ifdef TOOLS_ENABLED
#include "navigation_mesh_editor_plugin.h"
#endif

#ifndef _3D_DISABLED
NavigationMeshGenerator *_nav_mesh_generator = nullptr;
#endif

NavigationServer *new_server() {
	return memnew(GodotNavigationServer);
}

void register_navigation_types() {
	NavigationServerManager::set_default_server(new_server);

#ifndef _3D_DISABLED
	_nav_mesh_generator = memnew(NavigationMeshGenerator);
	ClassDB::register_class<NavigationMeshGenerator>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("NavigationMeshGenerator", NavigationMeshGenerator::get_singleton()));
#endif

#ifdef TOOLS_ENABLED
	EditorPlugins::add_by_type<NavigationMeshEditorPlugin>();
#endif
}

void unregister_navigation_types() {
#ifndef _3D_DISABLED
	if (_nav_mesh_generator) {
		memdelete(_nav_mesh_generator);
	}
#endif
}
