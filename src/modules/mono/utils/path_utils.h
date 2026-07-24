/**************************************************************************/
/*  path_utils.h                                                          */
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

#ifndef MONO_PATH_UTILS_H
#define MONO_PATH_UTILS_H

#include "core/string_builder.h"
#include "core/ustring.h"

namespace path {

String join(const String &p_a, const String &p_b);
String join(const String &p_a, const String &p_b, const String &p_c);
String join(const String &p_a, const String &p_b, const String &p_c, const String &p_d);

String find_executable(const String &p_name);

/// Returns a normalized absolute path to the current working directory
String cwd();

/**
 * Obtains a normalized absolute path to p_path. Symbolic links are
 * not resolved. The path p_path might not exist in the file system.
 */
String abspath(const String &p_path);

/**
 * Obtains a normalized path to p_path with symbolic links resolved.
 * The resulting path might be either a relative or an absolute path.
 */
String realpath(const String &p_path);

String relative_to(const String &p_path, const String &p_relative_to);

String get_csharp_project_name();

} // namespace path

#endif // MONO_PATH_UTILS_H
