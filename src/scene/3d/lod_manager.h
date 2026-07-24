/**************************************************************************/
/*  lod_manager.h                                                         */
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

#ifndef LOD_MANAGER_H
#define LOD_MANAGER_H

#include "core/local_vector.h"
#include "core/os/mutex.h"

class Camera;
class LOD;
struct Vector3;

class LODManager {
public:
	enum { NUM_LOD_QUEUES = 5 };

	void register_camera(Camera *p_camera);
	void remove_camera(Camera *p_camera);
	void register_lod(LOD *p_lod, uint32_t p_queue_id);
	void unregister_lod(LOD *p_lod, uint32_t p_queue_id);
	void update();

	void notify_saving(bool p_active);

	static void set_enabled(bool p_enabled) { _enabled = p_enabled; }
	static bool is_enabled() { return _enabled; }

private:
	void _update_queue(uint32_t p_queue_id, const Vector3 *p_camera_positions, uint32_t p_num_cameras);

	struct Queue {
		LocalVector<LOD *> lods;
		uint32_t lod_iterator = 0;
	};

	struct Data {
		LocalVector<Camera *> cameras;
		Queue queues[NUM_LOD_QUEUES];
		BinaryMutex mutex;
		bool saving = false;
	} data;

	static bool _enabled;
};

#endif // LOD_MANAGER_H
