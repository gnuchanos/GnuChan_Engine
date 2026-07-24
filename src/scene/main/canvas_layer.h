/**************************************************************************/
/*  canvas_layer.h                                                        */
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

#ifndef CANVAS_LAYER_H
#define CANVAS_LAYER_H

#include "scene/main/node.h"
#include "scene/resources/world_2d.h"

class Viewport;
class CanvasLayer : public Node {
	GDCLASS(CanvasLayer, Node);

	bool locrotscale_dirty;
	Vector2 ofs;
	Size2 scale;
	real_t rot;
	int layer;
	Transform2D transform;
	RID canvas;
	uint32_t _layer_order_in_tree;

	ObjectID custom_viewport_id; // to check validity
	Viewport *custom_viewport;

	RID viewport;
	Viewport *vp;

	int sort_index;
	bool visible;

	bool follow_viewport;
	float follow_viewport_scale;

	void _update_xform();
	void _update_locrotscale();
	void _update_follow_viewport(bool p_force_exit = false);
	void _calculate_layer_orders_in_tree(Node *p_node, uint32_t &r_order);
	void _update_layer_orders();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void update_draw_order();

	void set_layer(int p_xform);
	int get_layer() const;

	void set_visible(bool p_visible);
	bool is_visible() const;
	void show();
	void hide();

	void set_transform(const Transform2D &p_xform);
	Transform2D get_transform() const;
	Transform2D get_final_transform() const;

	void set_offset(const Vector2 &p_offset);
	Vector2 get_offset() const;

	void set_rotation(real_t p_radians);
	real_t get_rotation() const;

	void set_rotation_degrees(real_t p_degrees);
	real_t get_rotation_degrees() const;

	void set_scale(const Size2 &p_scale);
	Size2 get_scale() const;

	Size2 get_viewport_size() const;

	RID get_viewport() const;

	void set_custom_viewport(Node *p_viewport);
	Node *get_custom_viewport() const;

	void reset_sort_index();
	int get_sort_index();

	void set_follow_viewport(bool p_enable);
	bool is_following_viewport() const;

	void set_follow_viewport_scale(float p_ratio);
	float get_follow_viewport_scale() const;

	RID get_canvas() const;

#ifdef TOOLS_ENABLED
	StringName get_property_store_alias(const StringName &p_property) const;
#endif

	CanvasLayer();
	~CanvasLayer();
};

#endif // CANVAS_LAYER_H
