/**************************************************************************/
/*  flow_container.h                                                      */
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

#ifndef FLOW_CONTAINER_H
#define FLOW_CONTAINER_H

#include "scene/gui/container.h"

class FlowContainer : public Container {
	GDCLASS(FlowContainer, Container);

public:
	enum AlignMode {
		ALIGN_BEGIN,
		ALIGN_CENTER,
		ALIGN_END
	};

private:
	int cached_size;
	int cached_line_count;

	bool vertical;
	AlignMode align;

	void _resort();

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	int get_line_count() const;

	virtual Size2 get_minimum_size() const;

	FlowContainer(bool p_vertical = false);

	void set_alignment(AlignMode p_align);
	AlignMode get_alignment() const;
};

class HFlowContainer : public FlowContainer {
	GDCLASS(HFlowContainer, FlowContainer);

public:
	HFlowContainer() :
			FlowContainer(false) {}
};

class VFlowContainer : public FlowContainer {
	GDCLASS(VFlowContainer, FlowContainer);

public:
	VFlowContainer() :
			FlowContainer(true) {}
};

VARIANT_ENUM_CAST(FlowContainer::AlignMode);

#endif // FLOW_CONTAINER_H
