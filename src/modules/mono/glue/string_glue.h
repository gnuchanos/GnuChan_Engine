/**************************************************************************/
/*  string_glue.h                                                         */
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

#ifndef STRING_GLUE_H
#define STRING_GLUE_H

#ifdef MONO_GLUE_ENABLED

#include "../mono_gd/gd_mono_marshal.h"

MonoArray *godot_icall_String_md5_buffer(MonoString *p_str);

MonoString *godot_icall_String_md5_text(MonoString *p_str);

int godot_icall_String_rfind(MonoString *p_str, MonoString *p_what, int p_from);

int godot_icall_String_rfindn(MonoString *p_str, MonoString *p_what, int p_from);

MonoArray *godot_icall_String_sha256_buffer(MonoString *p_str);

MonoString *godot_icall_String_sha256_text(MonoString *p_str);

// Register internal calls

void godot_register_string_icalls();

#endif // MONO_GLUE_ENABLED

#endif // STRING_GLUE_H
