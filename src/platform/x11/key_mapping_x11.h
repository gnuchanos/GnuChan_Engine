/**************************************************************************/
/*  key_mapping_x11.h                                                     */
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

#ifndef KEY_MAPPING_X11_H
#define KEY_MAPPING_X11_H

#include <X11/XF86keysym.h>
#include <X11/Xlib.h>
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_XKB_KEYS
#include <X11/keysymdef.h>

#include "core/os/keyboard.h"

class KeyMappingX11 {
	KeyMappingX11(){};

public:
	static unsigned int get_keycode(KeySym p_keysym);
	static unsigned int get_xlibcode(unsigned int p_keysym);
	static unsigned int get_scancode(unsigned int p_code);
	static KeySym get_keysym(unsigned int p_code);
	static unsigned int get_unicode_from_keysym(KeySym p_keysym);
	static KeySym get_keysym_from_unicode(unsigned int p_unicode);
};

#endif // KEY_MAPPING_X11_H
