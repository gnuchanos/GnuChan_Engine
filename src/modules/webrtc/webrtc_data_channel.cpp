/**************************************************************************/
/*  webrtc_data_channel.cpp                                               */
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

#include "webrtc_data_channel.h"
#include "core/project_settings.h"

void WebRTCDataChannel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("poll"), &WebRTCDataChannel::poll);
	ClassDB::bind_method(D_METHOD("close"), &WebRTCDataChannel::close);

	ClassDB::bind_method(D_METHOD("was_string_packet"), &WebRTCDataChannel::was_string_packet);
	ClassDB::bind_method(D_METHOD("set_write_mode", "write_mode"), &WebRTCDataChannel::set_write_mode);
	ClassDB::bind_method(D_METHOD("get_write_mode"), &WebRTCDataChannel::get_write_mode);
	ClassDB::bind_method(D_METHOD("get_ready_state"), &WebRTCDataChannel::get_ready_state);
	ClassDB::bind_method(D_METHOD("get_label"), &WebRTCDataChannel::get_label);
	ClassDB::bind_method(D_METHOD("is_ordered"), &WebRTCDataChannel::is_ordered);
	ClassDB::bind_method(D_METHOD("get_id"), &WebRTCDataChannel::get_id);
	ClassDB::bind_method(D_METHOD("get_max_packet_life_time"), &WebRTCDataChannel::get_max_packet_life_time);
	ClassDB::bind_method(D_METHOD("get_max_retransmits"), &WebRTCDataChannel::get_max_retransmits);
	ClassDB::bind_method(D_METHOD("get_protocol"), &WebRTCDataChannel::get_protocol);
	ClassDB::bind_method(D_METHOD("is_negotiated"), &WebRTCDataChannel::is_negotiated);
	ClassDB::bind_method(D_METHOD("get_buffered_amount"), &WebRTCDataChannel::get_buffered_amount);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "write_mode", PROPERTY_HINT_ENUM), "set_write_mode", "get_write_mode");

	BIND_ENUM_CONSTANT(WRITE_MODE_TEXT);
	BIND_ENUM_CONSTANT(WRITE_MODE_BINARY);

	BIND_ENUM_CONSTANT(STATE_CONNECTING);
	BIND_ENUM_CONSTANT(STATE_OPEN);
	BIND_ENUM_CONSTANT(STATE_CLOSING);
	BIND_ENUM_CONSTANT(STATE_CLOSED);
}

WebRTCDataChannel::WebRTCDataChannel() {
	_in_buffer_shift = nearest_shift((int)GLOBAL_GET(WRTC_IN_BUF) - 1) + 10;
}

WebRTCDataChannel::~WebRTCDataChannel() {
}
