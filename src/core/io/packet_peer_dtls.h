/**************************************************************************/
/*  packet_peer_dtls.h                                                    */
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

#ifndef PACKET_PEER_DTLS_H
#define PACKET_PEER_DTLS_H

#include "core/crypto/crypto.h"
#include "core/io/packet_peer_udp.h"

class PacketPeerDTLS : public PacketPeer {
	GDCLASS(PacketPeerDTLS, PacketPeer);

protected:
	static PacketPeerDTLS *(*_create)();
	static void _bind_methods();

	static bool available;

public:
	enum Status {
		STATUS_DISCONNECTED,
		STATUS_HANDSHAKING,
		STATUS_CONNECTED,
		STATUS_ERROR,
		STATUS_ERROR_HOSTNAME_MISMATCH
	};

	virtual void poll() = 0;
	virtual Error connect_to_peer(Ref<PacketPeerUDP> p_base, bool p_validate_certs = true, const String &p_for_hostname = String(), Ref<X509Certificate> p_ca_certs = Ref<X509Certificate>()) = 0;
	virtual void disconnect_from_peer() = 0;
	virtual Status get_status() const = 0;

	static PacketPeerDTLS *create();
	static bool is_available();

	PacketPeerDTLS();
};

VARIANT_ENUM_CAST(PacketPeerDTLS::Status);

#endif // PACKET_PEER_DTLS_H
