/**************************************************************************/
/*  stream_peer_mbedtls.h                                                 */
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

#ifndef STREAM_PEER_MBEDTLS_H
#define STREAM_PEER_MBEDTLS_H

#include "core/io/stream_peer_ssl.h"
#include "ssl_context_mbedtls.h"

class StreamPeerMbedTLS : public StreamPeerSSL {
private:
	Status status;
	String hostname;

	Ref<StreamPeer> base;

	static StreamPeerSSL *_create_func();

	static int bio_recv(void *ctx, unsigned char *buf, size_t len);
	static int bio_send(void *ctx, const unsigned char *buf, size_t len);
	void _cleanup();

protected:
	Ref<SSLContextMbedTLS> ssl_ctx;

	Error _do_handshake();

public:
	virtual void poll();
	virtual Error accept_stream(Ref<StreamPeer> p_base, Ref<CryptoKey> p_key, Ref<X509Certificate> p_cert, Ref<X509Certificate> p_ca_chain = Ref<X509Certificate>());
	virtual Error connect_to_stream(Ref<StreamPeer> p_base, bool p_validate_certs = false, const String &p_for_hostname = String(), Ref<X509Certificate> p_valid_cert = Ref<X509Certificate>());
	virtual Status get_status() const;

	virtual void disconnect_from_stream();

	virtual Error put_data(const uint8_t *p_data, int p_bytes);
	virtual Error put_partial_data(const uint8_t *p_data, int p_bytes, int &r_sent);

	virtual Error get_data(uint8_t *p_buffer, int p_bytes);
	virtual Error get_partial_data(uint8_t *p_buffer, int p_bytes, int &r_received);

	virtual int get_available_bytes() const;

	static void initialize_ssl();
	static void finalize_ssl();

	StreamPeerMbedTLS();
	~StreamPeerMbedTLS();
};

#endif // STREAM_PEER_MBEDTLS_H
