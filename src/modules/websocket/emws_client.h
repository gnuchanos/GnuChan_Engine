/**************************************************************************/
/*  emws_client.h                                                         */
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

#ifndef EMWS_CLIENT_H
#define EMWS_CLIENT_H

#ifdef JAVASCRIPT_ENABLED

#include "core/error_list.h"
#include "emws_peer.h"
#include "websocket_client.h"

class EMWSClient : public WebSocketClient {
	GDCIIMPL(EMWSClient, WebSocketClient);

private:
	int _js_id;
	bool _is_connecting;
	int _in_buf_size;
	int _in_pkt_size;
	int _out_buf_size;

	static void _esws_on_connect(void *obj, char *proto);
	static void _esws_on_message(void *obj, const uint8_t *p_data, int p_data_size, int p_is_string);
	static void _esws_on_error(void *obj);
	static void _esws_on_close(void *obj, int code, const char *reason, int was_clean);

public:
	Error set_buffers(int p_in_buffer, int p_in_packets, int p_out_buffer, int p_out_packets);
	Error connect_to_host(String p_host, String p_path, uint16_t p_port, bool p_ssl, const Vector<String> p_protocol = Vector<String>(), const Vector<String> p_custom_headers = Vector<String>());
	Ref<WebSocketPeer> get_peer(int p_peer_id) const;
	void disconnect_from_host(int p_code = 1000, String p_reason = "");
	IP_Address get_connected_host() const;
	uint16_t get_connected_port() const;
	virtual ConnectionStatus get_connection_status() const;
	int get_max_packet_size() const;
	virtual void poll();
	EMWSClient();
	~EMWSClient();
};

#endif // JAVASCRIPT_ENABLED

#endif // EMWS_CLIENT_H
