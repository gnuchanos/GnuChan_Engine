/**************************************************************************/
/*  pane_drag.h                                                           */
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

#ifndef PANE_DRAG_H
#define PANE_DRAG_H

#include "scene/gui/control.h"

class PaneDrag : public Control {
	GDCLASS(PaneDrag, Control);

	bool mouse_over;

protected:
	void _gui_input(const Ref<InputEvent> &p_input);
	void _notification(int p_what);
	virtual Size2 get_minimum_size() const;
	static void _bind_methods();

public:
	PaneDrag();
};

#endif // PANE_DRAG_H
