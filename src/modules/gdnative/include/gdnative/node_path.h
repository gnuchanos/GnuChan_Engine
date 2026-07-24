/**************************************************************************/
/*  node_path.h                                                           */
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

#ifndef GDNATIVE_NODE_PATH_H
#define GDNATIVE_NODE_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GODOT_NODE_PATH_SIZE sizeof(void *)

#ifndef GODOT_CORE_API_GODOT_NODE_PATH_TYPE_DEFINED
#define GODOT_CORE_API_GODOT_NODE_PATH_TYPE_DEFINED
typedef struct {
	uint8_t _dont_touch_that[GODOT_NODE_PATH_SIZE];
} godot_node_path;
#endif

// reduce extern "C" nesting for VS2013
#ifdef __cplusplus
}
#endif

#include <gdnative/gdnative.h>
#include <gdnative/string.h>

#ifdef __cplusplus
extern "C" {
#endif

void GDAPI godot_node_path_new(godot_node_path *r_dest, const godot_string *p_from);
void GDAPI godot_node_path_new_copy(godot_node_path *r_dest, const godot_node_path *p_src);
void GDAPI godot_node_path_destroy(godot_node_path *p_self);

godot_string GDAPI godot_node_path_as_string(const godot_node_path *p_self);

godot_bool GDAPI godot_node_path_is_absolute(const godot_node_path *p_self);

godot_int GDAPI godot_node_path_get_name_count(const godot_node_path *p_self);

godot_string GDAPI godot_node_path_get_name(const godot_node_path *p_self, const godot_int p_idx);

godot_int GDAPI godot_node_path_get_subname_count(const godot_node_path *p_self);

godot_string GDAPI godot_node_path_get_subname(const godot_node_path *p_self, const godot_int p_idx);

godot_string GDAPI godot_node_path_get_concatenated_subnames(const godot_node_path *p_self);

godot_bool GDAPI godot_node_path_is_empty(const godot_node_path *p_self);

godot_bool GDAPI godot_node_path_operator_equal(const godot_node_path *p_self, const godot_node_path *p_b);

godot_node_path godot_node_path_get_as_property_path(const godot_node_path *p_self);

#ifdef __cplusplus
}
#endif

#endif // GDNATIVE_NODE_PATH_H
