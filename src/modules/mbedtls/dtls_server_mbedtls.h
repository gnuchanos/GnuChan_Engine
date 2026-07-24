/**************************************************************************/
/*  dtls_server_mbedtls.h                                                 */
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

#ifndef DTLS_SERVER_MBEDTLS_H
#define DTLS_SERVER_MBEDTLS_H

#include "core/io/dtls_server.h"
#include "ssl_context_mbedtls.h"

class DTLSServerMbedTLS : public DTLSServer {
private:
	static DTLSServer *_create_func();
	Ref<CryptoKey> _key;
	Ref<X509Certificate> _cert;
	Ref<X509Certificate> _ca_chain;
	Ref<CookieContextMbedTLS> _cookies;

public:
	static void initialize();
	static void finalize();

	virtual Error setup(Ref<CryptoKey> p_key, Ref<X509Certificate> p_cert, Ref<X509Certificate> p_ca_chain = Ref<X509Certificate>());
	virtual void stop();
	virtual Ref<PacketPeerDTLS> take_connection(Ref<PacketPeerUDP> p_peer);

	DTLSServerMbedTLS();
	~DTLSServerMbedTLS();
};

#endif // DTLS_SERVER_MBEDTLS_H
