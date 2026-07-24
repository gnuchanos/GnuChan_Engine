/**************************************************************************/
/*  webxr_interface.h                                                     */
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

#ifndef WEBXR_INTERFACE_H
#define WEBXR_INTERFACE_H

#include "servers/arvr/arvr_interface.h"
#include "servers/arvr/arvr_positional_tracker.h"

/**
	@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>

	The WebXR interface is a VR/AR interface that can be used on the web.
*/

class WebXRInterface : public ARVRInterface {
	GDCLASS(WebXRInterface, ARVRInterface);

protected:
	static void _bind_methods();

public:
	enum TargetRayMode {
		TARGET_RAY_MODE_UNKNOWN,
		TARGET_RAY_MODE_GAZE,
		TARGET_RAY_MODE_TRACKED_POINTER,
		TARGET_RAY_MODE_SCREEN,
	};

	virtual void is_session_supported(const String &p_session_mode) = 0;
	virtual void set_session_mode(String p_session_mode) = 0;
	virtual String get_session_mode() const = 0;
	virtual void set_required_features(String p_required_features) = 0;
	virtual String get_required_features() const = 0;
	virtual void set_optional_features(String p_optional_features) = 0;
	virtual String get_optional_features() const = 0;
	virtual void set_requested_reference_space_types(String p_requested_reference_space_types) = 0;
	virtual String get_requested_reference_space_types() const = 0;
	virtual String get_reference_space_type() const = 0;
	virtual Ref<ARVRPositionalTracker> get_controller(int p_controller_id) const = 0;
	virtual TargetRayMode get_controller_target_ray_mode(int p_controller_id) const = 0;
	virtual String get_visibility_state() const = 0;
	virtual PoolVector3Array get_bounds_geometry() const = 0;
	virtual void set_xr_standard_mapping(bool p_xr_standard_mapping) = 0;
	virtual bool get_xr_standard_mapping() const = 0;
};

VARIANT_ENUM_CAST(WebXRInterface::TargetRayMode);

#endif // WEBXR_INTERFACE_H
