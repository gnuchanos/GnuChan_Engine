/**************************************************************************/
/*  collision_solver_2d_sw.h                                              */
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

#ifndef COLLISION_SOLVER_2D_SW_H
#define COLLISION_SOLVER_2D_SW_H

#include "shape_2d_sw.h"

class CollisionSolver2DSW {
public:
	typedef void (*CallbackResult)(const Vector2 &p_point_A, const Vector2 &p_point_B, void *p_userdata);

private:
	static bool solve_static_line(const Shape2DSW *p_shape_A, const Transform2D &p_transform_A, const Shape2DSW *p_shape_B, const Transform2D &p_transform_B, CallbackResult p_result_callback, void *p_userdata, bool p_swap_result);
	static bool concave_callback(void *p_userdata, Shape2DSW *p_convex);
	static bool solve_concave(const Shape2DSW *p_shape_A, const Transform2D &p_transform_A, const Vector2 &p_motion_A, const Shape2DSW *p_shape_B, const Transform2D &p_transform_B, const Vector2 &p_motion_B, CallbackResult p_result_callback, void *p_userdata, bool p_swap_result, Vector2 *sep_axis = nullptr, real_t p_margin_A = 0, real_t p_margin_B = 0);
	static bool solve_raycast(const Shape2DSW *p_shape_A, const Vector2 &p_motion_A, const Transform2D &p_transform_A, const Shape2DSW *p_shape_B, const Transform2D &p_transform_B, CallbackResult p_result_callback, void *p_userdata, bool p_swap_result, Vector2 *sep_axis = nullptr, real_t p_margin = 0);

public:
	static bool solve(const Shape2DSW *p_shape_A, const Transform2D &p_transform_A, const Vector2 &p_motion_A, const Shape2DSW *p_shape_B, const Transform2D &p_transform_B, const Vector2 &p_motion_B, CallbackResult p_result_callback, void *p_userdata, Vector2 *sep_axis = nullptr, real_t p_margin_A = 0, real_t p_margin_B = 0);
};

#endif // COLLISION_SOLVER_2D_SW_H
