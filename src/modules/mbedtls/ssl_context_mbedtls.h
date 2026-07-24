/**************************************************************************/
/*  ssl_context_mbedtls.h                                                 */
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

#ifndef SSL_CONTEXT_MBEDTLS_H
#define SSL_CONTEXT_MBEDTLS_H

#include "crypto_mbedtls.h"

#include "core/os/file_access.h"
#include "core/pool_vector.h"
#include "core/reference.h"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ssl_cookie.h>

class SSLContextMbedTLS;

class CookieContextMbedTLS : public Reference {
	friend class SSLContextMbedTLS;

protected:
	bool inited;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_cookie_ctx cookie_ctx;

public:
	Error setup();
	void clear();

	CookieContextMbedTLS();
	~CookieContextMbedTLS();
};

class SSLContextMbedTLS : public Reference {
protected:
	bool inited;

public:
	static void print_mbedtls_error(int p_ret);

	Ref<X509CertificateMbedTLS> certs;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;

	Ref<CookieContextMbedTLS> cookies;
	Ref<CryptoKeyMbedTLS> pkey;

	Error _setup(int p_endpoint, int p_transport, int p_authmode);
	Error init_server(int p_transport, int p_authmode, Ref<CryptoKeyMbedTLS> p_pkey, Ref<X509CertificateMbedTLS> p_cert, Ref<CookieContextMbedTLS> p_cookies = Ref<CookieContextMbedTLS>());
	Error init_client(int p_transport, int p_authmode, Ref<X509CertificateMbedTLS> p_valid_cas);
	void clear();

	mbedtls_ssl_context *get_context();

	SSLContextMbedTLS();
	~SSLContextMbedTLS();
};

#endif // SSL_CONTEXT_MBEDTLS_H
