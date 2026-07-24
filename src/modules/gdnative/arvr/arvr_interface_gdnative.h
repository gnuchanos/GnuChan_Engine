/**************************************************************************/
/*  arvr_interface_gdnative.h                                             */
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

#ifndef ARVR_INTERFACE_GDNATIVE_H
#define ARVR_INTERFACE_GDNATIVE_H

#include "modules/gdnative/gdnative.h"
#include "servers/arvr/arvr_interface.h"

/**
	@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>

	This subclass of our AR/VR interface forms a bridge to GDNative.
*/

class ARVRInterfaceGDNative : public ARVRInterface {
	GDCLASS(ARVRInterfaceGDNative, ARVRInterface);

	void cleanup();

protected:
	const godot_arvr_interface_gdnative *interface;
	void *data;

	static void _bind_methods();

public:
	/** general interface information **/
	ARVRInterfaceGDNative();
	~ARVRInterfaceGDNative();

	void set_interface(const godot_arvr_interface_gdnative *p_interface);

	virtual StringName get_name() const;
	virtual int get_capabilities() const;

	virtual bool is_initialized() const;
	virtual bool initialize();
	virtual void uninitialize();

	/** specific to AR **/
	virtual bool get_anchor_detection_is_enabled() const;
	virtual void set_anchor_detection_is_enabled(bool p_enable);
	virtual int get_camera_feed_id();

	/** rendering and internal **/
	virtual Size2 get_render_targetsize();
	virtual bool is_stereo();
	virtual Transform get_transform_for_eye(ARVRInterface::Eyes p_eye, const Transform &p_cam_transform);

	// and a CameraMatrix version to ARVRServer
	virtual CameraMatrix get_projection_for_eye(ARVRInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far);

	virtual unsigned int get_external_texture_for_eye(ARVRInterface::Eyes p_eye);
	virtual unsigned int get_external_depth_for_eye(ARVRInterface::Eyes p_eye);
	virtual void commit_for_eye(ARVRInterface::Eyes p_eye, RID p_render_target, const Rect2 &p_screen_rect);

	virtual void process();
	virtual void notification(int p_what);
};

#endif // ARVR_INTERFACE_GDNATIVE_H
