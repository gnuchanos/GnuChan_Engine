/**************************************************************************/
/*  key_mapping_windows.h                                                 */
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

#ifndef KEY_MAPPING_WINDOWS_H
#define KEY_MAPPING_WINDOWS_H

#include "core/os/keyboard.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>

class KeyMappingWindows {
	KeyMappingWindows(){};

public:
	static unsigned int get_keysym(unsigned int p_code);
	static unsigned int get_scancode(unsigned int p_keycode);
	static unsigned int get_scansym(unsigned int p_code, bool p_extended);
	static bool is_extended_key(unsigned int p_code);
};

#endif // KEY_MAPPING_WINDOWS_H
