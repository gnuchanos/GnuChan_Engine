/**************************************************************************/
/*  pluginscript_loader.h                                                 */
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

#ifndef PLUGINSCRIPT_LOADER_H
#define PLUGINSCRIPT_LOADER_H

// GnuChan imports
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/script_language.h"

class PluginScriptLanguage;

class ResourceFormatLoaderPluginScript : public ResourceFormatLoader {
	PluginScriptLanguage *_language;

public:
	ResourceFormatLoaderPluginScript(PluginScriptLanguage *language);
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatSaverPluginScript : public ResourceFormatSaver {
	PluginScriptLanguage *_language;

public:
	ResourceFormatSaverPluginScript(PluginScriptLanguage *language);
	virtual Error save(const String &p_path, const RES &p_resource, uint32_t p_flags = 0);
	virtual void get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const;
	virtual bool recognize(const RES &p_resource) const;
};

#endif // PLUGINSCRIPT_LOADER_H
