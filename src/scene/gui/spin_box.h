/**************************************************************************/
/*  spin_box.h                                                            */
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

#ifndef SPIN_BOX_H
#define SPIN_BOX_H

#include "scene/gui/line_edit.h"
#include "scene/gui/range.h"
#include "scene/main/timer.h"

class SpinBox : public Range {
	GDCLASS(SpinBox, Range);

	LineEdit *line_edit;
	int last_w;

	Timer *range_click_timer;
	void _range_click_timeout();
	void _release_mouse();

	void _text_entered(const String &p_string);
	virtual void _value_changed(double);
	String prefix;
	String suffix;
	double custom_arrow_step = 0.0;

	void _line_edit_input(const Ref<InputEvent> &p_event);

	struct Drag {
		float base_val = 0;
		bool allowed = false;
		bool enabled = false;
		Vector2 capture_pos;
		float diff_y = 0;
	} drag;

	void _line_edit_focus_enter();
	void _line_edit_focus_exit();

	inline void _adjust_width_for_icon(const Ref<Texture> &icon);

protected:
	void _gui_input(const Ref<InputEvent> &p_event);

	void _notification(int p_what);

	static void _bind_methods();

public:
	LineEdit *get_line_edit();

	virtual Size2 get_minimum_size() const;

	void set_align(LineEdit::Align p_align);
	LineEdit::Align get_align() const;

	void set_editable(bool p_editable);
	bool is_editable() const;

	void set_suffix(const String &p_suffix);
	String get_suffix() const;

	void set_prefix(const String &p_prefix);
	String get_prefix() const;

	void apply();
	void set_custom_arrow_step(const double p_custom_arrow_step);
	double get_custom_arrow_step() const;

	SpinBox();
};

#endif // SPIN_BOX_H
