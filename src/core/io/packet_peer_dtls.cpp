/**************************************************************************/
/*  packet_peer_dtls.cpp                                                  */
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

#include "packet_peer_dtls.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"

PacketPeerDTLS *(*PacketPeerDTLS::_create)() = nullptr;
bool PacketPeerDTLS::available = false;

PacketPeerDTLS *PacketPeerDTLS::create() {
	if (_create) {
		return _create();
	}
	return nullptr;
}

bool PacketPeerDTLS::is_available() {
	return available;
}

void PacketPeerDTLS::_bind_methods() {
	ClassDB::bind_method(D_METHOD("poll"), &PacketPeerDTLS::poll);
	ClassDB::bind_method(D_METHOD("connect_to_peer", "packet_peer", "validate_certs", "for_hostname", "valid_certificate"), &PacketPeerDTLS::connect_to_peer, DEFVAL(true), DEFVAL(String()), DEFVAL(Ref<X509Certificate>()));
	ClassDB::bind_method(D_METHOD("get_status"), &PacketPeerDTLS::get_status);
	ClassDB::bind_method(D_METHOD("disconnect_from_peer"), &PacketPeerDTLS::disconnect_from_peer);

	BIND_ENUM_CONSTANT(STATUS_DISCONNECTED);
	BIND_ENUM_CONSTANT(STATUS_HANDSHAKING);
	BIND_ENUM_CONSTANT(STATUS_CONNECTED);
	BIND_ENUM_CONSTANT(STATUS_ERROR);
	BIND_ENUM_CONSTANT(STATUS_ERROR_HOSTNAME_MISMATCH);
}

PacketPeerDTLS::PacketPeerDTLS() {
}
