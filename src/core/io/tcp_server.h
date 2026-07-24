/**************************************************************************/
/*  tcp_server.h                                                          */
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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "core/io/ip.h"
#include "core/io/net_socket.h"
#include "core/io/stream_peer.h"
#include "core/io/stream_peer_tcp.h"

class TCP_Server : public Reference {
	GDCLASS(TCP_Server, Reference);

protected:
	enum {
		MAX_PENDING_CONNECTIONS = 8
	};

	Ref<NetSocket> _sock;
	static void _bind_methods();

public:
	Error listen(uint16_t p_port, const IP_Address &p_bind_address = IP_Address("*"));
	bool is_listening() const;
	bool is_connection_available() const;
	Ref<StreamPeerTCP> take_connection();

	void stop(); // Stop listening

	TCP_Server();
	~TCP_Server();
};

#endif // TCP_SERVER_H
