/**************************************************************************/
/*  grid_container.h                                                      */
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

#ifndef GRID_CONTAINER_H
#define GRID_CONTAINER_H

#include "scene/gui/container.h"

class GridContainer : public Container {
	GDCLASS(GridContainer, Container);

	int columns;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_columns(int p_columns);
	int get_columns() const;
	virtual Size2 get_minimum_size() const;

	GridContainer();
};

#endif // GRID_CONTAINER_H
