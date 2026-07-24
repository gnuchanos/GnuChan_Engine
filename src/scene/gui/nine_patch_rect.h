/**************************************************************************/
/*  nine_patch_rect.h                                                     */
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

#ifndef NINE_PATCH_RECT_H
#define NINE_PATCH_RECT_H

#include "scene/gui/control.h"

class NinePatchRect : public Control {
	GDCLASS(NinePatchRect, Control);

public:
	enum AxisStretchMode {
		AXIS_STRETCH_MODE_STRETCH,
		AXIS_STRETCH_MODE_TILE,
		AXIS_STRETCH_MODE_TILE_FIT,
	};

	bool draw_center;
	int margin[4];
	Rect2 region_rect;
	Ref<Texture> texture;

	AxisStretchMode axis_h, axis_v;

protected:
	void _notification(int p_what);
	virtual Size2 get_minimum_size() const;
	static void _bind_methods();

public:
	void set_texture(const Ref<Texture> &p_tex);
	Ref<Texture> get_texture() const;

	void set_patch_margin(Margin p_margin, int p_size);
	int get_patch_margin(Margin p_margin) const;

	void set_region_rect(const Rect2 &p_region_rect);
	Rect2 get_region_rect() const;

	void set_draw_center(bool p_enabled);
	bool is_draw_center_enabled() const;

	void set_h_axis_stretch_mode(AxisStretchMode p_mode);
	AxisStretchMode get_h_axis_stretch_mode() const;

	void set_v_axis_stretch_mode(AxisStretchMode p_mode);
	AxisStretchMode get_v_axis_stretch_mode() const;

	String get_configuration_warning() const;

	NinePatchRect();
	~NinePatchRect();
};

VARIANT_ENUM_CAST(NinePatchRect::AxisStretchMode)

#endif // NINE_PATCH_RECT_H
