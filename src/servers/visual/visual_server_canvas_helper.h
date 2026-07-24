/**************************************************************************/
/*  visual_server_canvas_helper.h                                         */
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

#ifndef VISUAL_SERVER_CANVAS_HELPER_H
#define VISUAL_SERVER_CANVAS_HELPER_H

#include "core/color.h"
#include "core/fixed_array.h"
#include "core/local_vector.h"
#include "core/math/rect2.h"
#include "core/rid.h"

class MultiRect;

class VisualServerCanvasHelper {
public:
	struct State {
		RID item;
		RID texture;
		Color modulate;
		RID normal_map;
		uint32_t flags = 0;

		bool operator==(const State &p_state) const {
			return ((item == p_state.item) &&
					(texture == p_state.texture) &&
					(modulate == p_state.modulate) &&
					(normal_map == p_state.normal_map) &&
					(flags == p_state.flags));
		}
		bool operator!=(const State &p_state) const { return !(*this == p_state); }
	};

private:
	// There is a single mutex for tilemaps, only one quadrant can be adding
	// at a time.
	static LocalVector<MultiRect> _tilemap_multirects;
	static Mutex _tilemap_mutex;

public:
	static void tilemap_begin();
	static void tilemap_add_rect(RID p_canvas_item, const Rect2 &p_rect, RID p_texture, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, RID p_normal_map = RID(), bool p_clip_uv = false);
	static void tilemap_end();

	static bool _multirect_enabled;
};

class MultiRect {
	friend class VisualServerCanvasHelper;

public:
	enum { MAX_RECTS = 2048 };

private:
	VisualServerCanvasHelper::State state;
	bool state_set = false;
	FixedArray<Rect2, MAX_RECTS, true> rects;
	FixedArray<Rect2, MAX_RECTS, true> sources;

	static uint32_t flags_from_rects(Rect2 &r_rect, Rect2 &r_source);
	bool overlaps(const Rect2 &p_rect) const {
		for (uint32_t n = 0; n < rects.size(); n++) {
			if (rects[n].intersects(p_rect)) {
				return true;
			}
		}
		return false;
	}
	bool add_pre_flipped(const Rect2 &p_rect, const Rect2 &p_src_rect);

public:
	// Simple API
	void add_rect(RID p_canvas_item, const Rect2 &p_rect, RID p_texture, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, RID p_normal_map = RID(), bool p_clip_uv = false);

	// Efficient API
	void begin(const VisualServerCanvasHelper::State &p_state);
	bool add(const Rect2 &p_rect, const Rect2 &p_src_rect, bool p_commit_on_flip_change = true);
	bool is_empty() const { return rects.is_empty(); }
	bool is_full() const { return rects.is_full(); }

	// Always called in destructor, but can be used explicitly
	// for multiple draws, or to time the flush.
	void flush();

	~MultiRect() { flush(); }
};

#endif // VISUAL_SERVER_CANVAS_HELPER_H
