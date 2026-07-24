/**************************************************************************/
/*  stream_peer_ssl.cpp                                                   */
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

#include "stream_peer_ssl.h"

#include "core/engine.h"

StreamPeerSSL *(*StreamPeerSSL::_create)() = nullptr;

StreamPeerSSL *StreamPeerSSL::create() {
	if (_create) {
		return _create();
	}
	return nullptr;
}

bool StreamPeerSSL::available = false;

bool StreamPeerSSL::is_available() {
	return available;
}

void StreamPeerSSL::set_blocking_handshake_enabled(bool p_enabled) {
	blocking_handshake = p_enabled;
}

bool StreamPeerSSL::is_blocking_handshake_enabled() const {
	return blocking_handshake;
}

void StreamPeerSSL::_bind_methods() {
	ClassDB::bind_method(D_METHOD("poll"), &StreamPeerSSL::poll);
	ClassDB::bind_method(D_METHOD("accept_stream", "stream", "private_key", "certificate", "chain"), &StreamPeerSSL::accept_stream, DEFVAL(Ref<X509Certificate>()));
	ClassDB::bind_method(D_METHOD("connect_to_stream", "stream", "validate_certs", "for_hostname", "valid_certificate"), &StreamPeerSSL::connect_to_stream, DEFVAL(false), DEFVAL(String()), DEFVAL(Ref<X509Certificate>()));
	ClassDB::bind_method(D_METHOD("get_status"), &StreamPeerSSL::get_status);
	ClassDB::bind_method(D_METHOD("disconnect_from_stream"), &StreamPeerSSL::disconnect_from_stream);
	ClassDB::bind_method(D_METHOD("set_blocking_handshake_enabled", "enabled"), &StreamPeerSSL::set_blocking_handshake_enabled);
	ClassDB::bind_method(D_METHOD("is_blocking_handshake_enabled"), &StreamPeerSSL::is_blocking_handshake_enabled);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "blocking_handshake"), "set_blocking_handshake_enabled", "is_blocking_handshake_enabled");

	BIND_ENUM_CONSTANT(STATUS_DISCONNECTED);
	BIND_ENUM_CONSTANT(STATUS_HANDSHAKING);
	BIND_ENUM_CONSTANT(STATUS_CONNECTED);
	BIND_ENUM_CONSTANT(STATUS_ERROR);
	BIND_ENUM_CONSTANT(STATUS_ERROR_HOSTNAME_MISMATCH);
}

StreamPeerSSL::StreamPeerSSL() {
	blocking_handshake = true;
}
