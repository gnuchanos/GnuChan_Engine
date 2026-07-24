/**************************************************************************/
/*  animation_cache.h                                                     */
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

#ifndef ANIMATION_CACHE_H
#define ANIMATION_CACHE_H

#include "scene/3d/skeleton.h"
#include "scene/resources/animation.h"

class AnimationCache : public Object {
	GDCLASS(AnimationCache, Object);

	struct Path {
		RES resource;
		Object *object;
		Skeleton *skeleton; // haxor
		Node *node;
		Spatial *spatial;

		int bone_idx;
		Vector<StringName> subpath;
		bool valid;
		Path() {
			object = nullptr;
			skeleton = nullptr;
			node = nullptr;
			bone_idx = -1;
			valid = false;
			spatial = nullptr;
		}
	};

	Set<Node *> connected_nodes;
	Vector<Path> path_cache;

	Node *root;
	Ref<Animation> animation;
	bool cache_dirty;
	bool cache_valid;

	void _node_exit_tree(Node *p_node);

	void _clear_cache();
	void _update_cache();
	void _animation_changed();

protected:
	static void _bind_methods();

public:
	void set_track_transform(int p_idx, const Transform &p_transform);
	void set_track_value(int p_idx, const Variant &p_value);
	void call_track(int p_idx, const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);

	void set_all(float p_time, float p_delta = 0);

	void set_animation(const Ref<Animation> &p_animation);
	void set_root(Node *p_root);

	AnimationCache();
};

#endif // ANIMATION_CACHE_H
