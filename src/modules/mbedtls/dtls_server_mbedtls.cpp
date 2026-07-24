/**************************************************************************/
/*  dtls_server_mbedtls.cpp                                               */
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

#include "dtls_server_mbedtls.h"
#include "packet_peer_mbed_dtls.h"

Error DTLSServerMbedTLS::setup(Ref<CryptoKey> p_key, Ref<X509Certificate> p_cert, Ref<X509Certificate> p_ca_chain) {
	ERR_FAIL_COND_V(_cookies->setup() != OK, ERR_ALREADY_IN_USE);
	_key = p_key;
	_cert = p_cert;
	_ca_chain = p_ca_chain;
	return OK;
}

void DTLSServerMbedTLS::stop() {
	_cookies->clear();
}

Ref<PacketPeerDTLS> DTLSServerMbedTLS::take_connection(Ref<PacketPeerUDP> p_udp_peer) {
	Ref<PacketPeerMbedDTLS> out;
	out.instance();

	ERR_FAIL_COND_V(!out.is_valid(), out);
	ERR_FAIL_COND_V(!p_udp_peer.is_valid(), out);
	out->accept_peer(p_udp_peer, _key, _cert, _ca_chain, _cookies);
	return out;
}

DTLSServer *DTLSServerMbedTLS::_create_func() {
	return memnew(DTLSServerMbedTLS);
}

void DTLSServerMbedTLS::initialize() {
	_create = _create_func;
	available = true;
}

void DTLSServerMbedTLS::finalize() {
	_create = nullptr;
	available = false;
}

DTLSServerMbedTLS::DTLSServerMbedTLS() {
	_cookies.instance();
}

DTLSServerMbedTLS::~DTLSServerMbedTLS() {
	stop();
}
