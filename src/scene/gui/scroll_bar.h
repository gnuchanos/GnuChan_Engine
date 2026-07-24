/**************************************************************************/
/*  scroll_bar.h                                                          */
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

#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "scene/gui/range.h"

class ScrollBar : public Range {
	GDCLASS(ScrollBar, Range);

	enum HighlightStatus {
		HIGHLIGHT_NONE,
		HIGHLIGHT_DECR,
		HIGHLIGHT_RANGE,
		HIGHLIGHT_INCR,
	};

	static bool focus_by_default;

	Orientation orientation;
	Size2 size;
	float custom_step;

	HighlightStatus highlight;

	bool incr_active = false;
	bool decr_active = false;

	struct Drag {
		bool active;
		float pos_at_click;
		float value_at_click;
	} drag;

	double get_grabber_size() const;
	double get_grabber_min_size() const;
	double get_area_size() const;
	double get_area_offset() const;
	double get_click_pos(const Point2 &p_pos) const;
	double get_grabber_offset() const;

	static void set_can_focus_by_default(bool p_can_focus);

	Node *drag_node;
	NodePath drag_node_path;
	bool drag_node_enabled;

	Vector2 drag_node_speed;
	Vector2 drag_node_accum;
	Vector2 drag_node_from;
	Vector2 last_drag_node_accum;
	float last_drag_node_time;
	float time_since_motion;
	bool drag_node_touching;
	bool drag_node_touching_deaccel;
	bool click_handled;

	bool scrolling;
	double target_scroll;
	bool smooth_scroll_enabled;

	void _drag_node_exit();
	void _drag_node_input(const Ref<InputEvent> &p_input);

	void _gui_input(Ref<InputEvent> p_event);

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	void set_custom_step(float p_custom_step);
	float get_custom_step() const;

	void set_drag_node(const NodePath &p_path);
	NodePath get_drag_node() const;
	void set_drag_node_enabled(bool p_enable);

	void set_smooth_scroll_enabled(bool p_enable);
	bool is_smooth_scroll_enabled() const;

	virtual Size2 get_minimum_size() const;
	ScrollBar(Orientation p_orientation = VERTICAL);
	~ScrollBar();
};

class HScrollBar : public ScrollBar {
	GDCLASS(HScrollBar, ScrollBar);

public:
	HScrollBar() :
			ScrollBar(HORIZONTAL) { set_v_size_flags(0); }
};

class VScrollBar : public ScrollBar {
	GDCLASS(VScrollBar, ScrollBar);

public:
	VScrollBar() :
			ScrollBar(VERTICAL) { set_h_size_flags(0); }
};

#endif // SCROLL_BAR_H
