/**************************************************************************/
/*  broad_phase_2d_bvh.h                                                  */
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

#ifndef BROAD_PHASE_2D_BVH_H
#define BROAD_PHASE_2D_BVH_H

#include "broad_phase_2d_sw.h"
#include "core/math/bvh.h"
#include "core/math/rect2.h"
#include "core/math/vector2.h"

class BroadPhase2DBVH : public BroadPhase2DSW {
	template <class T>
	class UserPairTestFunction {
	public:
		static bool user_pair_check(const T *p_a, const T *p_b) {
			// return false if no collision, decided by masks etc
			return p_a->test_collision_mask(p_b);
		}
	};

	template <class T>
	class UserCullTestFunction {
	public:
		static bool user_cull_check(const T *p_a, const T *p_b) {
			return true;
		}
	};

	enum Tree {
		TREE_STATIC = 0,
		TREE_DYNAMIC = 1,
	};

	enum TreeFlag {
		TREE_FLAG_STATIC = 1 << TREE_STATIC,
		TREE_FLAG_DYNAMIC = 1 << TREE_DYNAMIC,
	};

	BVH_Manager<CollisionObject2DSW, 2, true, 128, UserPairTestFunction<CollisionObject2DSW>, UserCullTestFunction<CollisionObject2DSW>, Rect2, Vector2> bvh;

	static void *_pair_callback(void *p_self, uint32_t p_id_A, CollisionObject2DSW *p_object_A, int p_subindex_A, uint32_t p_id_B, CollisionObject2DSW *p_object_B, int p_subindex_B);
	static void _unpair_callback(void *p_self, uint32_t p_id_A, CollisionObject2DSW *p_object_A, int p_subindex_A, uint32_t p_id_B, CollisionObject2DSW *p_object_B, int p_subindex_B, void *p_pair_data);
	static void *_check_pair_callback(void *p_self, uint32_t p_id_A, CollisionObject2DSW *p_object_A, int p_subindex_A, uint32_t p_id_B, CollisionObject2DSW *p_object_B, int p_subindex_B, void *p_pair_data);

	PairCallback pair_callback;
	void *pair_userdata;
	UnpairCallback unpair_callback;
	void *unpair_userdata;

public:
	// 0 is an invalid ID
	virtual ID create(CollisionObject2DSW *p_object, int p_subindex = 0, const Rect2 &p_aabb = Rect2(), bool p_static = false);
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
	BroadPhase2DBVH();
};

#endif // BROAD_PHASE_2D_BVH_H
