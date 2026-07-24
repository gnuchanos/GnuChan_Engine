/**************************************************************************/
/*  webrtc_gdnative.cpp                                                   */
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

#include "modules/gdnative/gdnative.h"
#include "modules/gdnative/include/net/godot_net.h"

#ifdef WEBRTC_GDNATIVE_ENABLED
#include "modules/webrtc/webrtc_data_channel_gdnative.h"
#include "modules/webrtc/webrtc_peer_connection_gdnative.h"
#endif

extern "C" {

void GDAPI godot_net_bind_webrtc_peer_connection(godot_object *p_obj, const godot_net_webrtc_peer_connection *p_impl) {
#ifdef WEBRTC_GDNATIVE_ENABLED
	((WebRTCPeerConnectionGDNative *)p_obj)->set_native_webrtc_peer_connection(p_impl);
#endif
}

void GDAPI godot_net_bind_webrtc_data_channel(godot_object *p_obj, const godot_net_webrtc_data_channel *p_impl) {
#ifdef WEBRTC_GDNATIVE_ENABLED
	((WebRTCDataChannelGDNative *)p_obj)->set_native_webrtc_data_channel(p_impl);
#endif
}

godot_error GDAPI godot_net_set_webrtc_library(const godot_net_webrtc_library *p_lib) {
#ifdef WEBRTC_GDNATIVE_ENABLED
	return (godot_error)WebRTCPeerConnectionGDNative::set_default_library(p_lib);
#else
	return (godot_error)ERR_UNAVAILABLE;
#endif
}
}
