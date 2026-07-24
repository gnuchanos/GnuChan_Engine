/**************************************************************************/
/*  mono_reg_utils.h                                                      */
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

#ifndef MONO_REG_UTILS_H
#define MONO_REG_UTILS_H

#ifdef WINDOWS_ENABLED

#include "core/ustring.h"

struct MonoRegInfo {
	String version;
	String install_root_dir;
	String assembly_dir;
	String config_dir;
	String bin_dir;
};

namespace MonoRegUtils {

MonoRegInfo find_mono();
String find_msbuild_tools_path();
} // namespace MonoRegUtils

#endif // WINDOWS_ENABLED

#endif // MONO_REG_UTILS_H
