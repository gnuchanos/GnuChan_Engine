/**************************************************************************/
/*  csharp_project.cpp                                                    */
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

#include "csharp_project.h"

#include "core/io/json.h"
#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "core/project_settings.h"

#include "../csharp_script.h"
#include "../mono_gd/gd_mono_class.h"
#include "../mono_gd/gd_mono_marshal.h"
#include "../utils/string_utils.h"
#include "script_class_parser.h"

namespace CSharpProject {

void add_item(const String &p_project_path, const String &p_item_type, const String &p_include) {
	if (!GLOBAL_DEF("mono/project/auto_update_project", true))
		return;

	GDMonoAssembly *tools_project_editor_assembly = GDMono::get_singleton()->get_tools_project_editor_assembly();

	GDMonoClass *klass = tools_project_editor_assembly->get_class("GodotTools.ProjectEditor", "ProjectUtils");

	Variant project_path = p_project_path;
	Variant item_type = p_item_type;
	Variant include = p_include;
	const Variant *args[3] = { &project_path, &item_type, &include };
	MonoException *exc = NULL;
	klass->get_method("AddItemToProjectChecked", 3)->invoke(NULL, args, &exc);

	if (exc) {
		GDMonoUtils::debug_print_unhandled_exception(exc);
		ERR_FAIL();
	}
}

} // namespace CSharpProject
