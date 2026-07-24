/**************************************************************************/
/*  packet_peer_gdnative.cpp                                              */
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

#include "packet_peer_gdnative.h"

PacketPeerGDNative::PacketPeerGDNative() {
	interface = nullptr;
}

PacketPeerGDNative::~PacketPeerGDNative() {
}

void PacketPeerGDNative::set_native_packet_peer(const godot_net_packet_peer *p_impl) {
	interface = p_impl;
}

void PacketPeerGDNative::_bind_methods() {
}

Error PacketPeerGDNative::get_packet(const uint8_t **r_buffer, int &r_buffer_size) {
	ERR_FAIL_COND_V(interface == nullptr, ERR_UNCONFIGURED);
	return (Error)interface->get_packet(interface->data, r_buffer, &r_buffer_size);
}

Error PacketPeerGDNative::put_packet(const uint8_t *p_buffer, int p_buffer_size) {
	ERR_FAIL_COND_V(interface == nullptr, ERR_UNCONFIGURED);
	return (Error)interface->put_packet(interface->data, p_buffer, p_buffer_size);
}

int PacketPeerGDNative::get_max_packet_size() const {
	ERR_FAIL_COND_V(interface == nullptr, 0);
	return interface->get_max_packet_size(interface->data);
}

int PacketPeerGDNative::get_available_packet_count() const {
	ERR_FAIL_COND_V(interface == nullptr, 0);
	return interface->get_available_packet_count(interface->data);
}

extern "C" {

void GDAPI godot_net_bind_packet_peer(godot_object *p_obj, const godot_net_packet_peer *p_impl) {
	((PacketPeerGDNative *)p_obj)->set_native_packet_peer(p_impl);
}
}
