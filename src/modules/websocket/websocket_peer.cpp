/**************************************************************************/
/*  websocket_peer.cpp                                                    */
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

#include "websocket_peer.h"

GDCINULL(WebSocketPeer);

WebSocketPeer::WebSocketPeer() {
}

WebSocketPeer::~WebSocketPeer() {
}

void WebSocketPeer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_write_mode"), &WebSocketPeer::get_write_mode);
	ClassDB::bind_method(D_METHOD("set_write_mode", "mode"), &WebSocketPeer::set_write_mode);
	ClassDB::bind_method(D_METHOD("is_connected_to_host"), &WebSocketPeer::is_connected_to_host);
	ClassDB::bind_method(D_METHOD("was_string_packet"), &WebSocketPeer::was_string_packet);
	ClassDB::bind_method(D_METHOD("close", "code", "reason"), &WebSocketPeer::close, DEFVAL(1000), DEFVAL(""));
	ClassDB::bind_method(D_METHOD("get_connected_host"), &WebSocketPeer::get_connected_host);
	ClassDB::bind_method(D_METHOD("get_connected_port"), &WebSocketPeer::get_connected_port);
	ClassDB::bind_method(D_METHOD("set_no_delay", "enabled"), &WebSocketPeer::set_no_delay);
	ClassDB::bind_method(D_METHOD("get_current_outbound_buffered_amount"), &WebSocketPeer::get_current_outbound_buffered_amount);

	BIND_ENUM_CONSTANT(WRITE_MODE_TEXT);
	BIND_ENUM_CONSTANT(WRITE_MODE_BINARY);
}
