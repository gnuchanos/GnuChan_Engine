/**************************************************************************/
/*  transform.h                                                           */
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

#ifndef GDNATIVE_TRANSFORM_H
#define GDNATIVE_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GODOT_TRANSFORM_SIZE 48

#ifndef GODOT_CORE_API_GODOT_TRANSFORM_TYPE_DEFINED
#define GODOT_CORE_API_GODOT_TRANSFORM_TYPE_DEFINED
typedef struct {
	uint8_t _dont_touch_that[GODOT_TRANSFORM_SIZE];
} godot_transform;
#endif

// reduce extern "C" nesting for VS2013
#ifdef __cplusplus
}
#endif

#include <gdnative/basis.h>
#include <gdnative/gdnative.h>
#include <gdnative/variant.h>
#include <gdnative/vector3.h>

#ifdef __cplusplus
extern "C" {
#endif

void GDAPI godot_transform_new_with_axis_origin(godot_transform *r_dest, const godot_vector3 *p_x_axis, const godot_vector3 *p_y_axis, const godot_vector3 *p_z_axis, const godot_vector3 *p_origin);
void GDAPI godot_transform_new(godot_transform *r_dest, const godot_basis *p_basis, const godot_vector3 *p_origin);
void GDAPI godot_transform_new_with_quat(godot_transform *r_dest, const godot_quat *p_quat);

godot_basis GDAPI godot_transform_get_basis(const godot_transform *p_self);
void GDAPI godot_transform_set_basis(godot_transform *p_self, const godot_basis *p_v);

godot_vector3 GDAPI godot_transform_get_origin(const godot_transform *p_self);
void GDAPI godot_transform_set_origin(godot_transform *p_self, const godot_vector3 *p_v);

godot_string GDAPI godot_transform_as_string(const godot_transform *p_self);

godot_transform GDAPI godot_transform_inverse(const godot_transform *p_self);

godot_transform GDAPI godot_transform_affine_inverse(const godot_transform *p_self);

godot_transform GDAPI godot_transform_orthonormalized(const godot_transform *p_self);

godot_transform GDAPI godot_transform_rotated(const godot_transform *p_self, const godot_vector3 *p_axis, const godot_real p_phi);

godot_transform GDAPI godot_transform_scaled(const godot_transform *p_self, const godot_vector3 *p_scale);

godot_transform GDAPI godot_transform_translated(const godot_transform *p_self, const godot_vector3 *p_ofs);

godot_transform GDAPI godot_transform_looking_at(const godot_transform *p_self, const godot_vector3 *p_target, const godot_vector3 *p_up);

godot_plane GDAPI godot_transform_xform_plane(const godot_transform *p_self, const godot_plane *p_v);

godot_plane GDAPI godot_transform_xform_inv_plane(const godot_transform *p_self, const godot_plane *p_v);

void GDAPI godot_transform_new_identity(godot_transform *r_dest);

godot_bool GDAPI godot_transform_operator_equal(const godot_transform *p_self, const godot_transform *p_b);

godot_transform GDAPI godot_transform_operator_multiply(const godot_transform *p_self, const godot_transform *p_b);

godot_vector3 GDAPI godot_transform_xform_vector3(const godot_transform *p_self, const godot_vector3 *p_v);

godot_vector3 GDAPI godot_transform_xform_inv_vector3(const godot_transform *p_self, const godot_vector3 *p_v);

godot_aabb GDAPI godot_transform_xform_aabb(const godot_transform *p_self, const godot_aabb *p_v);

godot_aabb GDAPI godot_transform_xform_inv_aabb(const godot_transform *p_self, const godot_aabb *p_v);

#ifdef __cplusplus
}
#endif

#endif // GDNATIVE_TRANSFORM_H
