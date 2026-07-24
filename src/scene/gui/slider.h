/**************************************************************************/
/*  slider.h                                                              */
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

#ifndef SLIDER_H
#define SLIDER_H

#include "scene/gui/range.h"

class Slider : public Range {
	GDCLASS(Slider, Range);

	struct Grab {
		int pos;
		float uvalue;
		bool active;
	} grab;

	int ticks;
	bool mouse_inside;
	Orientation orientation;
	float custom_step;
	bool editable;
	bool scrollable;

protected:
	void _gui_input(Ref<InputEvent> p_event);
	void _notification(int p_what);
	static void _bind_methods();
	bool ticks_on_borders;

public:
	virtual Size2 get_minimum_size() const;

	void set_custom_step(float p_custom_step);
	float get_custom_step() const;

	void set_ticks(int p_count);
	int get_ticks() const;

	void set_ticks_on_borders(bool);
	bool get_ticks_on_borders() const;

	void set_editable(bool p_editable);
	bool is_editable() const;

	void set_scrollable(bool p_scrollable);
	bool is_scrollable() const;

	Slider(Orientation p_orientation = VERTICAL);
};

class HSlider : public Slider {
	GDCLASS(HSlider, Slider);

public:
	HSlider() :
			Slider(HORIZONTAL) { set_v_size_flags(0); }
};

class VSlider : public Slider {
	GDCLASS(VSlider, Slider);

public:
	VSlider() :
			Slider(VERTICAL) { set_h_size_flags(0); }
};

#endif // SLIDER_H
