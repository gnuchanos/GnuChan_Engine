/**************************************************************************/
/*  godotsharp_dirs.h                                                     */
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

#ifndef GODOTSHARP_DIRS_H
#define GODOTSHARP_DIRS_H

#include "core/ustring.h"

namespace GodotSharpDirs {

String get_res_data_dir();
String get_res_metadata_dir();
String get_res_assemblies_base_dir();
String get_res_assemblies_dir();
String get_res_config_dir();
String get_res_temp_dir();
String get_res_temp_assemblies_base_dir();
String get_res_temp_assemblies_dir();

String get_mono_user_dir();
String get_mono_logs_dir();

#ifdef TOOLS_ENABLED
String get_mono_solutions_dir();
String get_build_logs_dir();

String get_project_assembly_name();
String get_project_sln_path();
String get_project_csproj_path();

String get_data_editor_tools_dir();
String get_data_editor_prebuilt_api_dir();
#else
String get_data_game_assemblies_dir();
#endif

String get_data_mono_etc_dir();
String get_data_mono_lib_dir();

#ifdef WINDOWS_ENABLED
String get_data_mono_bin_dir();
#endif

} // namespace GodotSharpDirs

#endif // GODOTSHARP_DIRS_H
