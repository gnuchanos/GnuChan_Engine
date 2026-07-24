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

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"

#include "nativescript.h"

#include "core/os/os.h"

NativeScriptLanguage *native_script_language;

Ref<ResourceFormatLoaderNativeScript> resource_loader_gdns;
Ref<ResourceFormatSaverNativeScript> resource_saver_gdns;

void register_nativescript_types() {
	native_script_language = memnew(NativeScriptLanguage);

	ClassDB::register_class<NativeScript>();

	native_script_language->set_language_index(ScriptServer::get_language_count());
	ScriptServer::register_language(native_script_language);

	resource_saver_gdns.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_gdns);

	resource_loader_gdns.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_gdns);
}

void unregister_nativescript_types() {
	ResourceLoader::remove_resource_format_loader(resource_loader_gdns);
	resource_loader_gdns.unref();

	ResourceSaver::remove_resource_format_saver(resource_saver_gdns);
	resource_saver_gdns.unref();

	if (native_script_language) {
		ScriptServer::unregister_language(native_script_language);
		memdelete(native_script_language);
	}
}
