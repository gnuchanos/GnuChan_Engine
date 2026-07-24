/**************************************************************************/
/*  rid_glue.h                                                            */
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

#ifndef RID_GLUE_H
#define RID_GLUE_H

#ifdef MONO_GLUE_ENABLED

#include "core/object.h"
#include "core/rid.h"

#include "../mono_gd/gd_mono_marshal.h"

RID *godot_icall_RID_Ctor(Object *p_from);

void godot_icall_RID_Dtor(RID *p_ptr);

uint32_t godot_icall_RID_get_id(RID *p_ptr);

// Register internal calls

void godot_register_rid_icalls();

#endif // MONO_GLUE_ENABLED

#endif // RID_GLUE_H
