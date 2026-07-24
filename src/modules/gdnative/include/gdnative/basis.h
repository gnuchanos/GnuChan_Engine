/**************************************************************************/
/*  basis.h                                                               */
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

#ifndef GDNATIVE_BASIS_H
#define GDNATIVE_BASIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GODOT_BASIS_SIZE 36

#ifndef GODOT_CORE_API_GODOT_BASIS_TYPE_DEFINED
#define GODOT_CORE_API_GODOT_BASIS_TYPE_DEFINED
typedef struct {
	uint8_t _dont_touch_that[GODOT_BASIS_SIZE];
} godot_basis;
#endif

// reduce extern "C" nesting for VS2013
#ifdef __cplusplus
}
#endif

#include <gdnative/gdnative.h>
#include <gdnative/quat.h>
#include <gdnative/vector3.h>

#ifdef __cplusplus
extern "C" {
#endif

void GDAPI godot_basis_new_with_rows(godot_basis *r_dest, const godot_vector3 *p_x_axis, const godot_vector3 *p_y_axis, const godot_vector3 *p_z_axis);
void GDAPI godot_basis_new_with_axis_and_angle(godot_basis *r_dest, const godot_vector3 *p_axis, const godot_real p_phi);
void GDAPI godot_basis_new_with_euler(godot_basis *r_dest, const godot_vector3 *p_euler);
void GDAPI godot_basis_new_with_euler_quat(godot_basis *r_dest, const godot_quat *p_euler);

godot_string GDAPI godot_basis_as_string(const godot_basis *p_self);

godot_basis GDAPI godot_basis_inverse(const godot_basis *p_self);

godot_basis GDAPI godot_basis_transposed(const godot_basis *p_self);

godot_basis GDAPI godot_basis_orthonormalized(const godot_basis *p_self);

godot_real GDAPI godot_basis_determinant(const godot_basis *p_self);

godot_basis GDAPI godot_basis_rotated(const godot_basis *p_self, const godot_vector3 *p_axis, const godot_real p_phi);

godot_basis GDAPI godot_basis_scaled(const godot_basis *p_self, const godot_vector3 *p_scale);

godot_vector3 GDAPI godot_basis_get_scale(const godot_basis *p_self);

godot_vector3 GDAPI godot_basis_get_euler(const godot_basis *p_self);

godot_quat GDAPI godot_basis_get_quat(const godot_basis *p_self);

void GDAPI godot_basis_set_quat(godot_basis *p_self, const godot_quat *p_quat);

void GDAPI godot_basis_set_axis_angle_scale(godot_basis *p_self, const godot_vector3 *p_axis, godot_real p_phi, const godot_vector3 *p_scale);

void GDAPI godot_basis_set_euler_scale(godot_basis *p_self, const godot_vector3 *p_euler, const godot_vector3 *p_scale);

void GDAPI godot_basis_set_quat_scale(godot_basis *p_self, const godot_quat *p_quat, const godot_vector3 *p_scale);

godot_real GDAPI godot_basis_tdotx(const godot_basis *p_self, const godot_vector3 *p_with);

godot_real GDAPI godot_basis_tdoty(const godot_basis *p_self, const godot_vector3 *p_with);

godot_real GDAPI godot_basis_tdotz(const godot_basis *p_self, const godot_vector3 *p_with);

godot_vector3 GDAPI godot_basis_xform(const godot_basis *p_self, const godot_vector3 *p_v);

godot_vector3 GDAPI godot_basis_xform_inv(const godot_basis *p_self, const godot_vector3 *p_v);

godot_int GDAPI godot_basis_get_orthogonal_index(const godot_basis *p_self);

void GDAPI godot_basis_new(godot_basis *r_dest);

// p_elements is a pointer to an array of 3 (!!) vector3
void GDAPI godot_basis_get_elements(const godot_basis *p_self, godot_vector3 *p_elements);

godot_vector3 GDAPI godot_basis_get_axis(const godot_basis *p_self, const godot_int p_axis);

void GDAPI godot_basis_set_axis(godot_basis *p_self, const godot_int p_axis, const godot_vector3 *p_value);

godot_vector3 GDAPI godot_basis_get_row(const godot_basis *p_self, const godot_int p_row);

void GDAPI godot_basis_set_row(godot_basis *p_self, const godot_int p_row, const godot_vector3 *p_value);

godot_bool GDAPI godot_basis_operator_equal(const godot_basis *p_self, const godot_basis *p_b);

godot_basis GDAPI godot_basis_operator_add(const godot_basis *p_self, const godot_basis *p_b);

godot_basis GDAPI godot_basis_operator_subtract(const godot_basis *p_self, const godot_basis *p_b);

godot_basis GDAPI godot_basis_operator_multiply_vector(const godot_basis *p_self, const godot_basis *p_b);

godot_basis GDAPI godot_basis_operator_multiply_scalar(const godot_basis *p_self, const godot_real p_b);

godot_basis GDAPI godot_basis_slerp(const godot_basis *p_self, const godot_basis *p_b, const godot_real p_t);

#ifdef __cplusplus
}
#endif

#endif // GDNATIVE_BASIS_H
