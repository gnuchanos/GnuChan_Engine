/**************************************************************************/
/*  step_2d_sw.h                                                          */
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

#ifndef STEP_2D_SW_H
#define STEP_2D_SW_H

#include "space_2d_sw.h"

class Step2DSW {
	uint64_t _step;

	void _populate_island(Body2DSW *p_body, Body2DSW **p_island, Constraint2DSW **p_constraint_island);
	bool _setup_island(Constraint2DSW *p_island, real_t p_delta);
	void _solve_island(Constraint2DSW *p_island, int p_iterations, real_t p_delta);
	void _check_suspend(Body2DSW *p_island, real_t p_delta);

public:
	void step(Space2DSW *p_space, real_t p_delta, int p_iterations);
	Step2DSW();
};

#endif // STEP_2D_SW_H
