/**************************************************************************/
/*  viewport_container.h                                                  */
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

#ifndef VIEWPORT_CONTAINER_H
#define VIEWPORT_CONTAINER_H

#include "scene/gui/container.h"

class ViewportContainer : public Container {
	GDCLASS(ViewportContainer, Container);

	bool stretch;
	int shrink;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_stretch(bool p_enable);
	bool is_stretch_enabled() const;

	void _input(const Ref<InputEvent> &p_event);
	void _unhandled_input(const Ref<InputEvent> &p_event);
	void set_stretch_shrink(int p_shrink);
	int get_stretch_shrink() const;

	virtual Size2 get_minimum_size() const;

	ViewportContainer();
};

#endif // VIEWPORT_CONTAINER_H
