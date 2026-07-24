/**************************************************************************/
/*  webrtc_peer_connection.h                                              */
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

#ifndef WEBRTC_PEER_CONNECTION_H
#define WEBRTC_PEER_CONNECTION_H

#include "core/io/packet_peer.h"
#include "modules/webrtc/webrtc_data_channel.h"

class WebRTCPeerConnection : public Reference {
	GDCLASS(WebRTCPeerConnection, Reference);

public:
	enum ConnectionState {
		STATE_NEW,
		STATE_CONNECTING,
		STATE_CONNECTED,
		STATE_DISCONNECTED,
		STATE_FAILED,
		STATE_CLOSED
	};

protected:
	static void _bind_methods();
	static WebRTCPeerConnection *(*_create)();

public:
	virtual ConnectionState get_connection_state() const = 0;

	virtual Error initialize(Dictionary p_config = Dictionary()) = 0;
	virtual Ref<WebRTCDataChannel> create_data_channel(String p_label, Dictionary p_options = Dictionary()) = 0;
	virtual Error create_offer() = 0;
	virtual Error set_remote_description(String type, String sdp) = 0;
	virtual Error set_local_description(String type, String sdp) = 0;
	virtual Error add_ice_candidate(String sdpMidName, int sdpMlineIndexName, String sdpName) = 0;
	virtual Error poll() = 0;
	virtual void close() = 0;

	static Ref<WebRTCPeerConnection> create_ref();
	static WebRTCPeerConnection *create();

	WebRTCPeerConnection();
	~WebRTCPeerConnection();
};

VARIANT_ENUM_CAST(WebRTCPeerConnection::ConnectionState);

#endif // WEBRTC_PEER_CONNECTION_H
