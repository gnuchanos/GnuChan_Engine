/**************************************************************************/
/*  box_container.h                                                       */
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

#ifndef BOX_CONTAINER_H
#define BOX_CONTAINER_H

#include "scene/gui/container.h"

class BoxContainer : public Container {
	GDCLASS(BoxContainer, Container);

	struct _MinSizeCache {
		int min_size;
		bool will_stretch;
		int final_size;
	};

public:
	enum AlignMode {
		ALIGN_BEGIN,
		ALIGN_CENTER,
		ALIGN_END
	};

private:
	bool vertical;
	AlignMode align;

	void _resort();

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	void add_spacer(bool p_begin = false);

	void set_alignment(AlignMode p_align);
	AlignMode get_alignment() const;

	virtual Size2 get_minimum_size() const;

	BoxContainer(bool p_vertical = false);
};

class HBoxContainer : public BoxContainer {
	GDCLASS(HBoxContainer, BoxContainer);

public:
	HBoxContainer() :
			BoxContainer(false) {}
};

class MarginContainer;
class VBoxContainer : public BoxContainer {
	GDCLASS(VBoxContainer, BoxContainer);

public:
	MarginContainer *add_margin_child(const String &p_label, Control *p_control, bool p_expand = false);

	VBoxContainer() :
			BoxContainer(true) {}
};

VARIANT_ENUM_CAST(BoxContainer::AlignMode);

#endif // BOX_CONTAINER_H
