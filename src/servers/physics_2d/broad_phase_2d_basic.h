/**************************************************************************/
/*  broad_phase_2d_basic.h                                                */
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

#ifndef BROAD_PHASE_2D_BASIC_H
#define BROAD_PHASE_2D_BASIC_H

#include "core/map.h"
#include "space_2d_sw.h"
class BroadPhase2DBasic : public BroadPhase2DSW {
	struct Element {
		CollisionObject2DSW *owner;
		bool _static;
		Rect2 aabb;
		int subindex;
	};

	Map<ID, Element> element_map;

	ID current;

	struct PairKey {
		union {
			struct {
				ID a;
				ID b;
			};
			uint64_t key;
		};

		_FORCE_INLINE_ bool operator<(const PairKey &p_key) const {
			return key < p_key.key;
		}

		PairKey() { key = 0; }
		PairKey(ID p_a, ID p_b) {
			if (p_a > p_b) {
				a = p_b;
				b = p_a;
			} else {
				a = p_a;
				b = p_b;
			}
		}
	};

	Map<PairKey, void *> pair_map;

	PairCallback pair_callback;
	void *pair_userdata;
	UnpairCallback unpair_callback;
	void *unpair_userdata;

public:
	// 0 is an invalid ID
	virtual ID create(CollisionObject2DSW *p_object_, int p_subindex = 0, const Rect2 &p_aabb = Rect2(), bool p_static = false);
	virtual void move(ID p_id, const Rect2 &p_aabb);
	virtual void recheck_pairs(ID p_id);
	virtual void set_static(ID p_id, bool p_static);
	virtual void remove(ID p_id);

	virtual CollisionObject2DSW *get_object(ID p_id) const;
	virtual bool is_static(ID p_id) const;
	virtual int get_subindex(ID p_id) const;

	virtual int cull_segment(const Vector2 &p_from, const Vector2 &p_to, CollisionObject2DSW **p_results, int p_max_results, int *p_result_indices = nullptr);
	virtual int cull_aabb(const Rect2 &p_aabb, CollisionObject2DSW **p_results, int p_max_results, int *p_result_indices = nullptr);

	virtual void set_pair_callback(PairCallback p_pair_callback, void *p_userdata);
	virtual void set_unpair_callback(UnpairCallback p_unpair_callback, void *p_userdata);

	virtual void update();

	static BroadPhase2DSW *_create();
	BroadPhase2DBasic();
};

#endif // BROAD_PHASE_2D_BASIC_H
