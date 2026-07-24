/**************************************************************************/
/*  multimesh_instance.h                                                  */
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

#ifndef MULTIMESH_INSTANCE_H
#define MULTIMESH_INSTANCE_H

#include "scene/3d/visual_instance.h"
#include "scene/resources/multimesh.h"

class MultiMeshInstance : public GeometryInstance {
	GDCLASS(MultiMeshInstance, GeometryInstance);

	Ref<MultiMesh> multimesh;

	void _refresh_interpolated();

protected:
	virtual void _physics_interpolated_changed();
	static void _bind_methods();
	void _notification(int p_what);
	// bind helpers

public:
	virtual PoolVector<Face3> get_faces(uint32_t p_usage_flags) const;

	void set_multimesh(const Ref<MultiMesh> &p_multimesh);
	Ref<MultiMesh> get_multimesh() const;

	virtual AABB get_aabb() const;

	MultiMeshInstance();
	~MultiMeshInstance();
};

#endif // MULTIMESH_INSTANCE_H
