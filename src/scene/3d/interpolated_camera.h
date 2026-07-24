/**************************************************************************/
/*  interpolated_camera.h                                                 */
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

#ifndef INTERPOLATED_CAMERA_H
#define INTERPOLATED_CAMERA_H

#include "scene/3d/camera.h"

class InterpolatedCamera : public Camera {
	GDCLASS(InterpolatedCamera, Camera);

public:
	enum InterpolatedCameraProcessMode {
		INTERPOLATED_CAMERA_PROCESS_PHYSICS,
		INTERPOLATED_CAMERA_PROCESS_IDLE
	};

private:
	bool enabled;
	real_t speed;
	NodePath target;
	InterpolatedCameraProcessMode process_mode;

	void _update_process_mode();

protected:
	void _notification(int p_what);
	static void _bind_methods();
	void _set_target(const Object *p_target);

public:
	void set_target(const Spatial *p_target);
	void set_target_path(const NodePath &p_path);
	NodePath get_target_path() const;

	void set_speed(real_t p_speed);
	real_t get_speed() const;

	void set_interpolation_enabled(bool p_enable);
	bool is_interpolation_enabled() const;

	void set_process_mode(InterpolatedCameraProcessMode p_mode);
	InterpolatedCameraProcessMode get_process_mode() const;

	InterpolatedCamera();
};

VARIANT_ENUM_CAST(InterpolatedCamera::InterpolatedCameraProcessMode);

#endif // INTERPOLATED_CAMERA_H
