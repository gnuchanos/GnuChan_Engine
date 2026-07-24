/**************************************************************************/
/*  websocket_client.h                                                    */
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

#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include "core/crypto/crypto.h"
#include "core/error_list.h"
#include "websocket_multiplayer_peer.h"
#include "websocket_peer.h"

class WebSocketClient : public WebSocketMultiplayerPeer {
	GDCLASS(WebSocketClient, WebSocketMultiplayerPeer);
	GDCICLASS(WebSocketClient);

protected:
	Ref<WebSocketPeer> _peer;
	bool verify_ssl;
	Ref<X509Certificate> ssl_cert;

	static void _bind_methods();

public:
	Error connect_to_url(String p_url, const Vector<String> p_protocols = Vector<String>(), bool gd_mp_api = false, const Vector<String> p_custom_headers = Vector<String>());

	void set_verify_ssl_enabled(bool p_verify_ssl);
	bool is_verify_ssl_enabled() const;
	Ref<X509Certificate> get_trusted_ssl_certificate() const;
	void set_trusted_ssl_certificate(Ref<X509Certificate> p_cert);

	virtual void poll() = 0;
	virtual Error connect_to_host(String p_host, String p_path, uint16_t p_port, bool p_ssl, const Vector<String> p_protocol = Vector<String>(), const Vector<String> p_custom_headers = Vector<String>()) = 0;
	virtual void disconnect_from_host(int p_code = 1000, String p_reason = "") = 0;
	virtual IP_Address get_connected_host() const = 0;
	virtual uint16_t get_connected_port() const = 0;

	virtual bool is_server() const;
	virtual ConnectionStatus get_connection_status() const = 0;

	void _on_peer_packet();
	void _on_connect(String p_protocol);
	void _on_close_request(int p_code, String p_reason);
	void _on_disconnect(bool p_was_clean);
	void _on_error();

	virtual Error set_buffers(int p_in_buffer, int p_in_packets, int p_out_buffer, int p_out_packets) = 0;

	WebSocketClient();
	~WebSocketClient();
};

#endif // WEBSOCKET_CLIENT_H
