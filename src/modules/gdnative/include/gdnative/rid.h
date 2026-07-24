/**************************************************************************/
/*  rid.h                                                                 */
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

#ifndef GDNATIVE_RID_H
#define GDNATIVE_RID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GODOT_RID_SIZE sizeof(void *)

#ifndef GODOT_CORE_API_GODOT_RID_TYPE_DEFINED
#define GODOT_CORE_API_GODOT_RID_TYPE_DEFINED
typedef struct {
	uint8_t _dont_touch_that[GODOT_RID_SIZE];
} godot_rid;
#endif

// reduce extern "C" nesting for VS2013
#ifdef __cplusplus
}
#endif

#include <gdnative/gdnative.h>

#ifdef __cplusplus
extern "C" {
#endif

void GDAPI godot_rid_new(godot_rid *r_dest);

godot_int GDAPI godot_rid_get_id(const godot_rid *p_self);

void GDAPI godot_rid_new_with_resource(godot_rid *r_dest, const godot_object *p_from);

godot_bool GDAPI godot_rid_operator_equal(const godot_rid *p_self, const godot_rid *p_b);

godot_bool GDAPI godot_rid_operator_less(const godot_rid *p_self, const godot_rid *p_b);

#ifdef __cplusplus
}
#endif

#endif // GDNATIVE_RID_H
