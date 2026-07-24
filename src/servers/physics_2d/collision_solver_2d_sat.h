/**************************************************************************/
/*  collision_solver_2d_sat.h                                             */
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

#ifndef COLLISION_SOLVER_2D_SAT_H
#define COLLISION_SOLVER_2D_SAT_H

#include "collision_solver_2d_sw.h"

bool sat_2d_calculate_penetration(const Shape2DSW *p_shape_A, const Transform2D &p_transform_A, const Vector2 &p_motion_A, const Shape2DSW *p_shape_B, const Transform2D &p_transform_B, const Vector2 &p_motion_B, CollisionSolver2DSW::CallbackResult p_result_callback, void *p_userdata, bool p_swap = false, Vector2 *sep_axis = nullptr, real_t p_margin_A = 0, real_t p_margin_B = 0);

#endif // COLLISION_SOLVER_2D_SAT_H
