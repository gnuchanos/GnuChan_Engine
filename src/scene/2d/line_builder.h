/**************************************************************************/
/*  line_builder.h                                                        */
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

#ifndef LINE_BUILDER_H
#define LINE_BUILDER_H

#include "core/color.h"
#include "core/math/vector2.h"
#include "line_2d.h"
#include "scene/resources/gradient.h"

class LineBuilder {
public:
	// TODO Move in a struct and reference it
	// Input
	Vector<Vector2> points;
	Line2D::LineJointMode joint_mode;
	Line2D::LineCapMode begin_cap_mode;
	Line2D::LineCapMode end_cap_mode;
	float width;
	Curve *curve;
	Color default_color;
	Gradient *gradient;
	Line2D::LineTextureMode texture_mode;
	float sharp_limit;
	int round_precision;
	float tile_aspect; // w/h
	// TODO offset_joints option (offers alternative implementation of round joints)

	// TODO Move in a struct and reference it
	// Output
	Vector<Vector2> vertices;
	Vector<Color> colors;
	Vector<Vector2> uvs;
	Vector<int> indices;

	LineBuilder();

	void build();
	void clear_output();

private:
	enum Orientation {
		UP = 0,
		DOWN = 1
	};

	// Triangle-strip methods
	void strip_begin(Vector2 up, Vector2 down, Color color, float uvx);
	void strip_new_quad(Vector2 up, Vector2 down, Color color, float uvx);
	void strip_add_quad(Vector2 up, Vector2 down, Color color, float uvx);
	void strip_add_tri(Vector2 up, Orientation orientation);
	void strip_add_arc(Vector2 center, float angle_delta, Orientation orientation);

	void new_arc(Vector2 center, Vector2 vbegin, float angle_delta, Color color, Rect2 uv_rect);

private:
	bool _interpolate_color;
	int _last_index[2]; // Index of last up and down vertices of the strip
};

#endif // LINE_BUILDER_H
