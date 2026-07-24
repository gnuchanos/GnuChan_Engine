/**************************************************************************/
/*  wsl_server.h                                                          */
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

#ifndef WSL_SERVER_H
#define WSL_SERVER_H

#ifndef JAVASCRIPT_ENABLED

#include "websocket_server.h"
#include "wsl_peer.h"

#include "core/io/stream_peer_ssl.h"
#include "core/io/stream_peer_tcp.h"
#include "core/io/tcp_server.h"

class WSLServer : public WebSocketServer {
	GDCIIMPL(WSLServer, WebSocketServer);

private:
	class PendingPeer : public Reference {
	private:
		bool _parse_request(const Vector<String> p_protocols);

	public:
		Ref<StreamPeerTCP> tcp;
		Ref<StreamPeer> connection;
		bool use_ssl;

		uint64_t time;
		uint8_t req_buf[WSL_MAX_HEADER_SIZE];
		int req_pos;
		String key;
		String protocol;
		bool has_request;
		CharString response;
		int response_sent;

		PendingPeer();

		Error do_handshake(const Vector<String> p_protocols, uint64_t p_timeout, const Vector<String> &p_extra_headers);
	};

	int _in_buf_size;
	int _in_pkt_size;
	int _out_buf_size;
	int _out_pkt_size;

	List<Ref<PendingPeer>> _pending;
	Ref<TCP_Server> _server;
	Vector<String> _protocols;
	Vector<String> _extra_headers;

public:
	Error set_buffers(int p_in_buffer, int p_in_packets, int p_out_buffer, int p_out_packets);
	void set_extra_headers(const Vector<String> &p_headers);
	Error listen(int p_port, const Vector<String> p_protocols = Vector<String>(), bool gd_mp_api = false);
	void stop();
	bool is_listening() const;
	int get_max_packet_size() const;
	bool has_peer(int p_id) const;
	Ref<WebSocketPeer> get_peer(int p_id) const;
	IP_Address get_peer_address(int p_peer_id) const;
	int get_peer_port(int p_peer_id) const;
	void disconnect_peer(int p_peer_id, int p_code = 1000, String p_reason = "");
	virtual void poll();

	WSLServer();
	~WSLServer();
};

#endif // JAVASCRIPT_ENABLED

#endif // WSL_SERVER_H
