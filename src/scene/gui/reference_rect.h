/**************************************************************************/
/*  reference_rect.h                                                      */
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

#ifndef REFERENCE_RECT_H
#define REFERENCE_RECT_H

#include "scene/gui/control.h"

class ReferenceRect : public Control {
	GDCLASS(ReferenceRect, Control);

	Color border_color = Color(1, 0, 0);
	float border_width = 1.0;
	bool editor_only = true;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_border_color(const Color &p_color);
	Color get_border_color() const;

	void set_border_width(float p_width);
	float get_border_width() const;

	void set_editor_only(const bool &p_enabled);
	bool get_editor_only() const;
};

#endif // REFERENCE_RECT_H
