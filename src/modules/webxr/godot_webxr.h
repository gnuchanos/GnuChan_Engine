/**************************************************************************/
/*  godot_webxr.h                                                         */
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

#ifndef GODOT_WEBXR_H
#define GODOT_WEBXR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"

enum WebXRInputEvent {
	WEBXR_INPUT_EVENT_SELECTSTART,
	WEBXR_INPUT_EVENT_SELECTEND,
	WEBXR_INPUT_EVENT_SELECT,
	WEBXR_INPUT_EVENT_SQUEEZESTART,
	WEBXR_INPUT_EVENT_SQUEEZEEND,
	WEBXR_INPUT_EVENT_SQUEEZE,
};

typedef void (*GodotWebXRSupportedCallback)(char *p_session_mode, int p_supported);
typedef void (*GodotWebXRStartedCallback)(char *p_reference_space_type);
typedef void (*GodotWebXREndedCallback)();
typedef void (*GodotWebXRFailedCallback)(char *p_message);
typedef void (*GodotWebXRControllerCallback)();
typedef void (*GodotWebXRInputEventCallback)(int p_event_type, int p_controller_id);
typedef void (*GodotWebXRSimpleEventCallback)(char *p_signal_name);

extern int godot_webxr_is_supported();
extern void godot_webxr_is_session_supported(const char *p_session_mode, GodotWebXRSupportedCallback p_callback);

extern void godot_webxr_initialize(
		const char *p_session_mode,
		const char *p_required_features,
		const char *p_optional_features,
		const char *p_requested_reference_space_types,
		GodotWebXRStartedCallback p_on_session_started,
		GodotWebXREndedCallback p_on_session_ended,
		GodotWebXRFailedCallback p_on_session_failed,
		GodotWebXRControllerCallback p_on_controller_changed,
		GodotWebXRInputEventCallback p_on_input_event,
		GodotWebXRSimpleEventCallback p_on_simple_event);
extern void godot_webxr_uninitialize();

extern int godot_webxr_get_view_count();
extern int *godot_webxr_get_render_targetsize();
extern float *godot_webxr_get_transform_for_eye(int p_eye);
extern float *godot_webxr_get_projection_for_eye(int p_eye);
extern void godot_webxr_commit_for_eye(int p_eye, unsigned int p_texture_id);

extern void godot_webxr_sample_controller_data();
extern int godot_webxr_get_controller_count();
extern int godot_webxr_is_controller_connected(int p_controller);
extern float *godot_webxr_get_controller_transform(int p_controller);
extern int *godot_webxr_get_controller_buttons(int p_controller, bool p_xr_standard_mapping);
extern int *godot_webxr_get_controller_axes(int p_controller, bool p_xr_standard_mapping);
extern int godot_webxr_get_controller_target_ray_mode(int p_controller);

extern char *godot_webxr_get_visibility_state();
extern int *godot_webxr_get_bounds_geometry();

#ifdef __cplusplus
}
#endif

#endif // GODOT_WEBXR_H
