/**************************************************************************/
/*  global_constants.h                                                    */
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

#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

#include "core/string_name.h"

class GlobalConstants {
public:
	static int get_global_constant_count();
	static StringName get_global_constant_enum(int p_idx);
	static bool get_ignore_value_in_docs(int p_idx);
	static const char *get_global_constant_name(int p_idx);
	static int get_global_constant_value(int p_idx);
};

#endif // GLOBAL_CONSTANTS_H
