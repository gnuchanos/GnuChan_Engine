/**************************************************************************/
/*  default_theme.h                                                       */
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

#ifndef DEFAULT_THEME_H
#define DEFAULT_THEME_H

#include "scene/resources/theme.h"

void fill_default_theme(Ref<Theme> &theme, const Ref<Font> &default_font, const Ref<Font> &large_font, Ref<Texture> &default_icon, Ref<StyleBox> &default_style, float p_scale);
void make_default_theme(bool p_hidpi, Ref<Font> p_font);
void clear_default_theme();

#endif // DEFAULT_THEME_H
