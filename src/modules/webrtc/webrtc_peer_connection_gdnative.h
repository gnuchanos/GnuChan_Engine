/**************************************************************************/
/*  webrtc_peer_connection_gdnative.h                                     */
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

#ifndef WEBRTC_PEER_CONNECTION_GDNATIVE_H
#define WEBRTC_PEER_CONNECTION_GDNATIVE_H

#ifdef WEBRTC_GDNATIVE_ENABLED

#include "modules/gdnative/include/net/godot_net.h"
#include "webrtc_peer_connection.h"

class WebRTCPeerConnectionGDNative : public WebRTCPeerConnection {
	GDCLASS(WebRTCPeerConnectionGDNative, WebRTCPeerConnection);

protected:
	static void _bind_methods();
	static WebRTCPeerConnection *_create();

private:
	static const godot_net_webrtc_library *default_library;
	const godot_net_webrtc_peer_connection *interface;

public:
	static Error set_default_library(const godot_net_webrtc_library *p_library);
	static void make_default() { WebRTCPeerConnection::_create = WebRTCPeerConnectionGDNative::_create; }

	void set_native_webrtc_peer_connection(const godot_net_webrtc_peer_connection *p_impl);

	virtual ConnectionState get_connection_state() const;

	virtual Error initialize(Dictionary p_config = Dictionary());
	virtual Ref<WebRTCDataChannel> create_data_channel(String p_label, Dictionary p_options = Dictionary());
	virtual Error create_offer();
	virtual Error set_remote_description(String type, String sdp);
	virtual Error set_local_description(String type, String sdp);
	virtual Error add_ice_candidate(String sdpMidName, int sdpMlineIndexName, String sdpName);
	virtual Error poll();
	virtual void close();

	WebRTCPeerConnectionGDNative();
	~WebRTCPeerConnectionGDNative();
};

#endif // WEBRTC_GDNATIVE_ENABLED

#endif // WEBRTC_PEER_CONNECTION_GDNATIVE_H
