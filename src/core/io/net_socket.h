/**************************************************************************/
/*  net_socket.h                                                          */
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

#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include "core/io/ip.h"
#include "core/reference.h"

class NetSocket : public Reference {
protected:
	static NetSocket *(*_create)();

public:
	static NetSocket *create();

	enum PollType {
		POLL_TYPE_IN,
		POLL_TYPE_OUT,
		POLL_TYPE_IN_OUT
	};

	enum Type {
		TYPE_NONE,
		TYPE_TCP,
		TYPE_UDP,
	};

	virtual Error open(Type p_type, IP::Type &ip_type) = 0;
	virtual void close() = 0;
	virtual Error bind(IP_Address p_addr, uint16_t p_port) = 0;
	virtual Error listen(int p_max_pending) = 0;
	virtual Error connect_to_host(IP_Address p_addr, uint16_t p_port) = 0;
	virtual Error poll(PollType p_type, int timeout) const = 0;
	virtual Error recv(uint8_t *p_buffer, int p_len, int &r_read) = 0;
	virtual Error recvfrom(uint8_t *p_buffer, int p_len, int &r_read, IP_Address &r_ip, uint16_t &r_port, bool p_peek = false) = 0;
	virtual Error send(const uint8_t *p_buffer, int p_len, int &r_sent) = 0;
	virtual Error sendto(const uint8_t *p_buffer, int p_len, int &r_sent, IP_Address p_ip, uint16_t p_port) = 0;
	virtual Ref<NetSocket> accept(IP_Address &r_ip, uint16_t &r_port) = 0;

	virtual bool is_open() const = 0;
	virtual int get_available_bytes() const = 0;

	virtual Error set_broadcasting_enabled(bool p_enabled) = 0; // Returns OK if the socket option has been set successfully.
	virtual void set_blocking_enabled(bool p_enabled) = 0;
	virtual void set_ipv6_only_enabled(bool p_enabled) = 0;
	virtual void set_tcp_no_delay_enabled(bool p_enabled) = 0;
	virtual void set_reuse_address_enabled(bool p_enabled) = 0;
	virtual Error join_multicast_group(const IP_Address &p_multi_address, String p_if_name) = 0;
	virtual Error leave_multicast_group(const IP_Address &p_multi_address, String p_if_name) = 0;
};

#endif // NET_SOCKET_H
