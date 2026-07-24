/**************************************************************************/
/*  networked_multiplayer_peer.cpp                                        */
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

#include "networked_multiplayer_peer.h"

void NetworkedMultiplayerPeer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_transfer_mode", "mode"), &NetworkedMultiplayerPeer::set_transfer_mode);
	ClassDB::bind_method(D_METHOD("get_transfer_mode"), &NetworkedMultiplayerPeer::get_transfer_mode);
	ClassDB::bind_method(D_METHOD("set_target_peer", "id"), &NetworkedMultiplayerPeer::set_target_peer);

	ClassDB::bind_method(D_METHOD("get_packet_peer"), &NetworkedMultiplayerPeer::get_packet_peer);

	ClassDB::bind_method(D_METHOD("poll"), &NetworkedMultiplayerPeer::poll);

	ClassDB::bind_method(D_METHOD("get_connection_status"), &NetworkedMultiplayerPeer::get_connection_status);
	ClassDB::bind_method(D_METHOD("get_unique_id"), &NetworkedMultiplayerPeer::get_unique_id);

	ClassDB::bind_method(D_METHOD("set_refuse_new_connections", "enable"), &NetworkedMultiplayerPeer::set_refuse_new_connections);
	ClassDB::bind_method(D_METHOD("is_refusing_new_connections"), &NetworkedMultiplayerPeer::is_refusing_new_connections);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "refuse_new_connections"), "set_refuse_new_connections", "is_refusing_new_connections");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "transfer_mode", PROPERTY_HINT_ENUM, "Unreliable,Unreliable Ordered,Reliable"), "set_transfer_mode", "get_transfer_mode");

	BIND_ENUM_CONSTANT(TRANSFER_MODE_UNRELIABLE);
	BIND_ENUM_CONSTANT(TRANSFER_MODE_UNRELIABLE_ORDERED);
	BIND_ENUM_CONSTANT(TRANSFER_MODE_RELIABLE);

	BIND_ENUM_CONSTANT(CONNECTION_DISCONNECTED);
	BIND_ENUM_CONSTANT(CONNECTION_CONNECTING);
	BIND_ENUM_CONSTANT(CONNECTION_CONNECTED);

	BIND_CONSTANT(TARGET_PEER_BROADCAST);
	BIND_CONSTANT(TARGET_PEER_SERVER);

	ADD_SIGNAL(MethodInfo("peer_connected", PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("peer_disconnected", PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("server_disconnected"));
	ADD_SIGNAL(MethodInfo("connection_succeeded"));
	ADD_SIGNAL(MethodInfo("connection_failed"));
}

NetworkedMultiplayerPeer::NetworkedMultiplayerPeer() {
}
