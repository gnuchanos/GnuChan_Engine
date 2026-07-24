/**************************************************************************/
/*  world_2d.h                                                            */
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

#ifndef WORLD_2D_H
#define WORLD_2D_H

#include "core/project_settings.h"
#include "core/resource.h"
#include "servers/physics_2d_server.h"

class VisibilityNotifier2D;
class Viewport;
struct SpatialIndexer2D;

class World2D : public Resource {
	GDCLASS(World2D, Resource);

	RID canvas;
	RID space;
	RID navigation_map;

	SpatialIndexer2D *indexer;

protected:
	static void _bind_methods();
	friend class Viewport;
	friend class VisibilityNotifier2D;

	void _register_viewport(Viewport *p_viewport, const Rect2 &p_rect);
	void _update_viewport(Viewport *p_viewport, const Rect2 &p_rect);
	void _remove_viewport(Viewport *p_viewport);

	void _register_notifier(VisibilityNotifier2D *p_notifier, const Rect2 &p_rect);
	void _update_notifier(VisibilityNotifier2D *p_notifier, const Rect2 &p_rect);
	void _remove_notifier(VisibilityNotifier2D *p_notifier);

	void _update();

public:
	RID get_canvas();
	RID get_space();
	RID get_navigation_map() const;

	Physics2DDirectSpaceState *get_direct_space_state();

	void get_viewport_list(List<Viewport *> *r_viewports);

	World2D();
	~World2D();
};

#endif // WORLD_2D_H
