/**************************************************************************/
/*  area_pair_2d_sw.h                                                     */
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

#ifndef AREA_PAIR_2D_SW_H
#define AREA_PAIR_2D_SW_H

#include "area_2d_sw.h"
#include "body_2d_sw.h"
#include "constraint_2d_sw.h"

class AreaPair2DSW : public Constraint2DSW {
	Body2DSW *body;
	Area2DSW *area;
	int body_shape;
	int area_shape;
	bool colliding;

public:
	bool setup(real_t p_step);
	void solve(real_t p_step);

	AreaPair2DSW(Body2DSW *p_body, int p_body_shape, Area2DSW *p_area, int p_area_shape);
	~AreaPair2DSW();
};

class Area2Pair2DSW : public Constraint2DSW {
	Area2DSW *area_a;
	Area2DSW *area_b;
	int shape_a;
	int shape_b;
	bool colliding;
	bool area_a_monitorable;
	bool area_b_monitorable;

public:
	bool setup(real_t p_step);
	void solve(real_t p_step);

	Area2Pair2DSW(Area2DSW *p_area_a, int p_shape_a, Area2DSW *p_area_b, int p_shape_b);
	~Area2Pair2DSW();
};

#endif // AREA_PAIR_2D_SW_H
