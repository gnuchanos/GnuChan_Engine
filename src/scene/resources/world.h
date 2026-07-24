/**************************************************************************/
/*  world.h                                                               */
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

#ifndef WORLD_H
#define WORLD_H

#include "core/resource.h"
#include "scene/resources/environment.h"
#include "servers/physics_server.h"
#include "servers/visual_server.h"

class Camera;
class VisibilityNotifier;
class LOD;
struct SpatialIndexer;
class LODManager;

class World : public Resource {
	GDCLASS(World, Resource);
	RES_BASE_EXTENSION("world");

private:
	RID space;
	RID scenario;
	RID navigation_map;

	SpatialIndexer *indexer;
	LODManager *lod_manager = nullptr;
	Ref<Environment> environment;
	Ref<Environment> fallback_environment;

protected:
	static void _bind_methods();

	friend class Camera;
	friend class VisibilityNotifier;
	friend class LOD;

	void _register_camera(Camera *p_camera);
	void _update_camera(Camera *p_camera);
	void _remove_camera(Camera *p_camera);

	void _register_notifier(VisibilityNotifier *p_notifier, const AABB &p_rect);
	void _update_notifier(VisibilityNotifier *p_notifier, const AABB &p_rect);
	void _remove_notifier(VisibilityNotifier *p_notifier);

	void _register_lod(LOD *p_lod, uint32_t p_queue_id);
	void _unregister_lod(LOD *p_lod, uint32_t p_queue_id);

	friend class Viewport;
	void _update(uint64_t p_frame);

public:
	RID get_space() const;
	RID get_scenario() const;
	RID get_navigation_map() const;

	void set_environment(const Ref<Environment> &p_environment);
	Ref<Environment> get_environment() const;

	void set_fallback_environment(const Ref<Environment> &p_environment);
	Ref<Environment> get_fallback_environment() const;

	void get_camera_list(List<Camera *> *r_cameras);

	PhysicsDirectSpaceState *get_direct_space_state();

	void notify_saving(bool p_active);

	World();
	~World();
};

#endif // WORLD_H
