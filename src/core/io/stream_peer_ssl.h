/**************************************************************************/
/*  stream_peer_ssl.h                                                     */
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

#ifndef STREAM_PEER_SSL_H
#define STREAM_PEER_SSL_H

#include "core/crypto/crypto.h"
#include "core/io/stream_peer.h"

class StreamPeerSSL : public StreamPeer {
	GDCLASS(StreamPeerSSL, StreamPeer);

protected:
	static StreamPeerSSL *(*_create)();
	static void _bind_methods();

	static bool available;

	bool blocking_handshake;

public:
	enum Status {
		STATUS_DISCONNECTED,
		STATUS_HANDSHAKING,
		STATUS_CONNECTED,
		STATUS_ERROR,
		STATUS_ERROR_HOSTNAME_MISMATCH
	};

	void set_blocking_handshake_enabled(bool p_enabled);
	bool is_blocking_handshake_enabled() const;

	virtual void poll() = 0;
	virtual Error accept_stream(Ref<StreamPeer> p_base, Ref<CryptoKey> p_key, Ref<X509Certificate> p_cert, Ref<X509Certificate> p_ca_chain = Ref<X509Certificate>()) = 0;
	virtual Error connect_to_stream(Ref<StreamPeer> p_base, bool p_validate_certs = false, const String &p_for_hostname = String(), Ref<X509Certificate> p_valid_cert = Ref<X509Certificate>()) = 0;
	virtual Status get_status() const = 0;

	virtual void disconnect_from_stream() = 0;

	static StreamPeerSSL *create();

	static bool is_available();

	StreamPeerSSL();
};

VARIANT_ENUM_CAST(StreamPeerSSL::Status);

#endif // STREAM_PEER_SSL_H
