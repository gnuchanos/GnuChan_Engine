/**************************************************************************/
/*  parallax_background.h                                                 */
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

#ifndef PARALLAX_BACKGROUND_H
#define PARALLAX_BACKGROUND_H

#include "scene/2d/camera_2d.h"
#include "scene/2d/node_2d.h"
#include "scene/main/canvas_layer.h"

class ParallaxBackground : public CanvasLayer {
	GDCLASS(ParallaxBackground, CanvasLayer);

	Point2 offset;
	float scale;
	Point2 base_offset;
	Point2 base_scale;
	Point2 screen_offset;
	String group_name;
	Point2 limit_begin;
	Point2 limit_end;
	Point2 final_offset;
	bool ignore_camera_zoom;

	void _update_scroll();

protected:
	void _camera_moved(const Transform2D &p_transform, const Point2 &p_screen_offset);

	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_scroll_offset(const Point2 &p_ofs);
	Point2 get_scroll_offset() const;

	void set_scroll_scale(float p_scale);
	float get_scroll_scale() const;

	void set_scroll_base_offset(const Point2 &p_ofs);
	Point2 get_scroll_base_offset() const;

	void set_scroll_base_scale(const Point2 &p_ofs);
	Point2 get_scroll_base_scale() const;

	void set_limit_begin(const Point2 &p_ofs);
	Point2 get_limit_begin() const;

	void set_limit_end(const Point2 &p_ofs);
	Point2 get_limit_end() const;

	void set_ignore_camera_zoom(bool ignore);
	bool is_ignore_camera_zoom();

	Vector2 get_final_offset() const;

	ParallaxBackground();
};

#endif // PARALLAX_BACKGROUND_H
