/**************************************************************************/
/*  transform2d.h                                                         */
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

#ifndef GDNATIVE_TRANSFORM2D_H
#define GDNATIVE_TRANSFORM2D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GODOT_TRANSFORM2D_SIZE 24

#ifndef GODOT_CORE_API_GODOT_TRANSFORM2D_TYPE_DEFINED
#define GODOT_CORE_API_GODOT_TRANSFORM2D_TYPE_DEFINED
typedef struct {
	uint8_t _dont_touch_that[GODOT_TRANSFORM2D_SIZE];
} godot_transform2d;
#endif

// reduce extern "C" nesting for VS2013
#ifdef __cplusplus
}
#endif

#include <gdnative/gdnative.h>
#include <gdnative/variant.h>
#include <gdnative/vector2.h>

#ifdef __cplusplus
extern "C" {
#endif

void GDAPI godot_transform2d_new(godot_transform2d *r_dest, const godot_real p_rot, const godot_vector2 *p_pos);
void GDAPI godot_transform2d_new_axis_origin(godot_transform2d *r_dest, const godot_vector2 *p_x_axis, const godot_vector2 *p_y_axis, const godot_vector2 *p_origin);

godot_string GDAPI godot_transform2d_as_string(const godot_transform2d *p_self);

godot_transform2d GDAPI godot_transform2d_inverse(const godot_transform2d *p_self);

godot_transform2d GDAPI godot_transform2d_affine_inverse(const godot_transform2d *p_self);

godot_real GDAPI godot_transform2d_get_rotation(const godot_transform2d *p_self);

godot_vector2 GDAPI godot_transform2d_get_origin(const godot_transform2d *p_self);

godot_vector2 GDAPI godot_transform2d_get_scale(const godot_transform2d *p_self);

godot_real GDAPI godot_transform2d_determinant(const godot_transform2d *p_self);

godot_transform2d GDAPI godot_transform2d_orthonormalized(const godot_transform2d *p_self);

godot_transform2d GDAPI godot_transform2d_rotated(const godot_transform2d *p_self, const godot_real p_phi);

godot_transform2d GDAPI godot_transform2d_scaled(const godot_transform2d *p_self, const godot_vector2 *p_scale);

godot_transform2d GDAPI godot_transform2d_translated(const godot_transform2d *p_self, const godot_vector2 *p_offset);

godot_vector2 GDAPI godot_transform2d_xform_vector2(const godot_transform2d *p_self, const godot_vector2 *p_v);

godot_vector2 GDAPI godot_transform2d_xform_inv_vector2(const godot_transform2d *p_self, const godot_vector2 *p_v);

godot_vector2 GDAPI godot_transform2d_basis_xform_vector2(const godot_transform2d *p_self, const godot_vector2 *p_v);

godot_vector2 GDAPI godot_transform2d_basis_xform_inv_vector2(const godot_transform2d *p_self, const godot_vector2 *p_v);

godot_transform2d GDAPI godot_transform2d_interpolate_with(const godot_transform2d *p_self, const godot_transform2d *p_m, const godot_real p_c);

godot_bool GDAPI godot_transform2d_operator_equal(const godot_transform2d *p_self, const godot_transform2d *p_b);

godot_transform2d GDAPI godot_transform2d_operator_multiply(const godot_transform2d *p_self, const godot_transform2d *p_b);

void GDAPI godot_transform2d_new_identity(godot_transform2d *r_dest);

godot_rect2 GDAPI godot_transform2d_xform_rect2(const godot_transform2d *p_self, const godot_rect2 *p_v);

godot_rect2 GDAPI godot_transform2d_xform_inv_rect2(const godot_transform2d *p_self, const godot_rect2 *p_v);

#ifdef __cplusplus
}
#endif

#endif // GDNATIVE_TRANSFORM2D_H
