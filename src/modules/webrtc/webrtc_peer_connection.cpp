/**************************************************************************/
/*  webrtc_peer_connection.cpp                                            */
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

#include "webrtc_peer_connection.h"

WebRTCPeerConnection *(*WebRTCPeerConnection::_create)() = nullptr;

Ref<WebRTCPeerConnection> WebRTCPeerConnection::create_ref() {
	return create();
}

WebRTCPeerConnection *WebRTCPeerConnection::create() {
	if (!_create) {
		return nullptr;
	}
	return _create();
}

void WebRTCPeerConnection::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "configuration"), &WebRTCPeerConnection::initialize, DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("create_data_channel", "label", "options"), &WebRTCPeerConnection::create_data_channel, DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("create_offer"), &WebRTCPeerConnection::create_offer);
	ClassDB::bind_method(D_METHOD("set_local_description", "type", "sdp"), &WebRTCPeerConnection::set_local_description);
	ClassDB::bind_method(D_METHOD("set_remote_description", "type", "sdp"), &WebRTCPeerConnection::set_remote_description);
	ClassDB::bind_method(D_METHOD("add_ice_candidate", "media", "index", "name"), &WebRTCPeerConnection::add_ice_candidate);
	ClassDB::bind_method(D_METHOD("poll"), &WebRTCPeerConnection::poll);
	ClassDB::bind_method(D_METHOD("close"), &WebRTCPeerConnection::close);

	ClassDB::bind_method(D_METHOD("get_connection_state"), &WebRTCPeerConnection::get_connection_state);

	ADD_SIGNAL(MethodInfo("session_description_created", PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::STRING, "sdp")));
	ADD_SIGNAL(MethodInfo("ice_candidate_created", PropertyInfo(Variant::STRING, "media"), PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::STRING, "name")));
	ADD_SIGNAL(MethodInfo("data_channel_received", PropertyInfo(Variant::OBJECT, "channel")));

	BIND_ENUM_CONSTANT(STATE_NEW);
	BIND_ENUM_CONSTANT(STATE_CONNECTING);
	BIND_ENUM_CONSTANT(STATE_CONNECTED);
	BIND_ENUM_CONSTANT(STATE_DISCONNECTED);
	BIND_ENUM_CONSTANT(STATE_FAILED);
	BIND_ENUM_CONSTANT(STATE_CLOSED);
}

WebRTCPeerConnection::WebRTCPeerConnection() {
}

WebRTCPeerConnection::~WebRTCPeerConnection() {
}
