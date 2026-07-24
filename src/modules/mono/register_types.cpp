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

#include "csharp_script.h"

CSharpLanguage *script_language_cs = NULL;
Ref<ResourceFormatLoaderCSharpScript> resource_loader_cs;
Ref<ResourceFormatSaverCSharpScript> resource_saver_cs;

_GodotSharp *_godotsharp = NULL;

void register_mono_types() {
	ClassDB::register_class<CSharpScript>();

	_godotsharp = memnew(_GodotSharp);

	ClassDB::register_class<_GodotSharp>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("GodotSharp", _GodotSharp::get_singleton()));

	script_language_cs = memnew(CSharpLanguage);
	script_language_cs->set_language_index(ScriptServer::get_language_count());
	ScriptServer::register_language(script_language_cs);

	resource_loader_cs.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_cs);

	resource_saver_cs.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_cs);
}

void unregister_mono_types() {
	ScriptServer::unregister_language(script_language_cs);

	if (script_language_cs)
		memdelete(script_language_cs);

	ResourceLoader::remove_resource_format_loader(resource_loader_cs);
	resource_loader_cs.unref();

	ResourceSaver::remove_resource_format_saver(resource_saver_cs);
	resource_saver_cs.unref();

	if (_godotsharp)
		memdelete(_godotsharp);
}
