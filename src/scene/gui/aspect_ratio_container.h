/**************************************************************************/
/*  aspect_ratio_container.h                                              */
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

#ifndef ASPECT_RATIO_CONTAINER_H
#define ASPECT_RATIO_CONTAINER_H

#include "scene/gui/container.h"

class AspectRatioContainer : public Container {
	GDCLASS(AspectRatioContainer, Container);

protected:
	void _notification(int p_what);
	static void _bind_methods();
	virtual Size2 get_minimum_size() const;

public:
	enum StretchMode {
		STRETCH_WIDTH_CONTROLS_HEIGHT,
		STRETCH_HEIGHT_CONTROLS_WIDTH,
		STRETCH_FIT,
		STRETCH_COVER,
	};
	enum AlignMode {
		ALIGN_BEGIN,
		ALIGN_CENTER,
		ALIGN_END,
	};

private:
	float ratio = 1.0;
	StretchMode stretch_mode = STRETCH_FIT;
	AlignMode alignment_horizontal = ALIGN_CENTER;
	AlignMode alignment_vertical = ALIGN_CENTER;

public:
	void set_ratio(float p_ratio);
	float get_ratio() const { return ratio; }

	void set_stretch_mode(StretchMode p_mode);
	StretchMode get_stretch_mode() const { return stretch_mode; }

	void set_alignment_horizontal(AlignMode p_alignment_horizontal);
	AlignMode get_alignment_horizontal() const { return alignment_horizontal; }

	void set_alignment_vertical(AlignMode p_alignment_vertical);
	AlignMode get_alignment_vertical() const { return alignment_vertical; }
};

VARIANT_ENUM_CAST(AspectRatioContainer::StretchMode);
VARIANT_ENUM_CAST(AspectRatioContainer::AlignMode);

#endif // ASPECT_RATIO_CONTAINER_H
