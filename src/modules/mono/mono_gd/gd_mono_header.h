/**************************************************************************/
/*  gd_mono_header.h                                                      */
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

#ifndef GD_MONO_HEADER_H
#define GD_MONO_HEADER_H

#include "core/int_types.h"

#ifdef WIN32
#define GD_MONO_STDCALL __stdcall
#else
#define GD_MONO_STDCALL
#endif

class GDMonoAssembly;
class GDMonoClass;
class GDMonoField;
class GDMonoMethod;
class GDMonoProperty;

class IMonoClassMember;

#include "managed_type.h"

#endif // GD_MONO_HEADER_H
