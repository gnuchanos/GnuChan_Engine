/**************************************************************************/
/*  occluder.h                                                            */
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

#ifndef OCCLUDER_H
#define OCCLUDER_H

#include "scene/3d/spatial.h"
#include "scene/resources/occluder_shape.h"

class Occluder : public Spatial {
	GDCLASS(Occluder, Spatial);

	friend class OccluderSpatialGizmo;
	friend class OccluderEditorPlugin;

	RID _occluder_instance;
	Ref<OccluderShape> _shape;

	void resource_changed(RES res);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_shape(const Ref<OccluderShape> &p_shape);
	Ref<OccluderShape> get_shape() const;

	String get_configuration_warning() const;

#ifdef TOOLS_ENABLED
	// for editor gizmo
	virtual AABB get_fallback_gizmo_aabb() const;
#endif

	Occluder();
	~Occluder();
};

#endif // OCCLUDER_H
