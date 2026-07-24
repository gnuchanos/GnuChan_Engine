/**************************************************************************/
/*  parallax_layer.h                                                      */
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

#ifndef PARALLAX_LAYER_H
#define PARALLAX_LAYER_H

#include "scene/2d/node_2d.h"

class ParallaxLayer : public Node2D {
	GDCLASS(ParallaxLayer, Node2D);

	Point2 orig_offset;
	Point2 orig_scale;
	Size2 motion_scale;
	Vector2 motion_offset;
	Vector2 mirroring;
	void _update_mirroring();

	Point2 screen_offset;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_motion_offset(const Size2 &p_offset);
	Size2 get_motion_offset() const;

	void set_motion_scale(const Size2 &p_scale);
	Size2 get_motion_scale() const;

	void set_mirroring(const Size2 &p_mirroring);
	Size2 get_mirroring() const;

	void set_base_offset_and_scale(const Point2 &p_offset, float p_scale, const Point2 &p_screen_offset);

	virtual String get_configuration_warning() const;
	ParallaxLayer();
};

#endif // PARALLAX_LAYER_H
