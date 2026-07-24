/**************************************************************************/
/*  skeleton_2d.h                                                         */
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

#ifndef SKELETON_2D_H
#define SKELETON_2D_H

#include "scene/2d/node_2d.h"

class Skeleton2D;

class Bone2D : public Node2D {
	GDCLASS(Bone2D, Node2D);

	friend class Skeleton2D;
#ifdef TOOLS_ENABLED
	friend class AnimatedValuesBackup;
#endif

	Bone2D *parent_bone;
	Skeleton2D *skeleton;
	Transform2D rest;
	float default_length;

	int skeleton_index;

protected:
	void _notification(int p_what);
	static void _bind_methods();
	void _order_changed_in_parent();

public:
	void set_rest(const Transform2D &p_rest);
	Transform2D get_rest() const;
	void apply_rest();
	Transform2D get_skeleton_rest() const;

	String get_configuration_warning() const;

	void set_default_length(float p_length);
	float get_default_length() const;

	int get_index_in_skeleton() const;

	Bone2D();
};

class Skeleton2D : public Node2D {
	GDCLASS(Skeleton2D, Node2D);

	friend class Bone2D;
#ifdef TOOLS_ENABLED
	friend class AnimatedValuesBackup;
#endif

	struct Bone {
		bool operator<(const Bone &p_bone) const {
			return p_bone.bone->is_greater_than(bone);
		}
		Bone2D *bone;
		int parent_index;
		Transform2D accum_transform;
		Transform2D rest_inverse;
	};

	Vector<Bone> bones;

	bool bone_setup_dirty;
	void _make_bone_setup_dirty();
	void _update_bone_setup();

	bool transform_dirty;
	void _make_transform_dirty();
	void _update_transform();

	RID skeleton;

	void _update_process_mode();
	void _ensure_update_interpolation_data();

	struct InterpolationData {
		Transform2D xform_curr;
		Transform2D xform_prev;
		uint32_t last_update_physics_tick = UINT32_MAX;
	} _interpolation_data;

protected:
	void _notification(int p_what);
	static void _bind_methods();

	virtual void _physics_interpolated_changed();

public:
	int get_bone_count() const;
	Bone2D *get_bone(int p_idx);

	RID get_skeleton() const;
	Skeleton2D();
	~Skeleton2D();
};

#endif // SKELETON_2D_H
