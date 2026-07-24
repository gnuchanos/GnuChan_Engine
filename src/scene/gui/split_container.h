/**************************************************************************/
/*  split_container.h                                                     */
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

#ifndef SPLIT_CONTAINER_H
#define SPLIT_CONTAINER_H

#include "scene/gui/container.h"

class SplitContainer : public Container {
	GDCLASS(SplitContainer, Container);

public:
	enum DraggerVisibility {
		DRAGGER_VISIBLE,
		DRAGGER_HIDDEN,
		DRAGGER_HIDDEN_COLLAPSED
	};

private:
	bool should_clamp_split_offset;
	int split_offset;
	int middle_sep;
	bool vertical;
	bool dragging;
	int drag_from;
	int drag_ofs;
	bool collapsed;
	DraggerVisibility dragger_visibility;
	bool mouse_inside;

	Control *_getch(int p_idx) const;

	void _resort();

protected:
	void _gui_input(const Ref<InputEvent> &p_event);
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_split_offset(int p_offset);
	int get_split_offset() const;
	void clamp_split_offset();

	void set_collapsed(bool p_collapsed);
	bool is_collapsed() const;

	void set_dragger_visibility(DraggerVisibility p_visibility);
	DraggerVisibility get_dragger_visibility() const;

	virtual CursorShape get_cursor_shape(const Point2 &p_pos = Point2i()) const;

	virtual Size2 get_minimum_size() const;

	SplitContainer(bool p_vertical = false);
};

VARIANT_ENUM_CAST(SplitContainer::DraggerVisibility);

class HSplitContainer : public SplitContainer {
	GDCLASS(HSplitContainer, SplitContainer);

public:
	HSplitContainer() :
			SplitContainer(false) {}
};

class VSplitContainer : public SplitContainer {
	GDCLASS(VSplitContainer, SplitContainer);

public:
	VSplitContainer() :
			SplitContainer(true) {}
};

#endif // SPLIT_CONTAINER_H
