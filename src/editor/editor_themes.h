/**************************************************************************/
/*  editor_themes.h                                                       */
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

#ifndef EDITOR_THEMES_H
#define EDITOR_THEMES_H

#include "scene/resources/texture.h"
#include "scene/resources/theme.h"

Ref<Theme> create_editor_theme(Ref<Theme> p_theme = nullptr);

Ref<Theme> create_custom_theme(Ref<Theme> p_theme = nullptr);

Ref<ImageTexture> create_unscaled_default_project_icon();

#endif // EDITOR_THEMES_H
