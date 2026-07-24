/**************************************************************************/
/*  godotsharp_export.h                                                   */
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

#ifndef GODOTSHARP_EXPORT_H
#define GODOTSHARP_EXPORT_H

#include "core/dictionary.h"
#include "core/error_list.h"
#include "core/ustring.h"

#include "../mono_gd/gd_mono_header.h"

namespace GodotSharpExport {

Error get_assembly_dependencies(GDMonoAssembly *p_assembly, const Vector<String> &p_search_dirs, Dictionary &r_dependencies);

Error get_exported_assembly_dependencies(const Dictionary &p_initial_assemblies,
		const String &p_build_config, const String &p_custom_lib_dir, Dictionary &r_assembly_dependencies);

} // namespace GodotSharpExport

#endif // GODOTSHARP_EXPORT_H
