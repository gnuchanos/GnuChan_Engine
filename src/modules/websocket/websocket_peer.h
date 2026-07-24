/**************************************************************************/
/*  websocket_peer.h                                                      */
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

#ifndef WEBSOCKET_PEER_H
#define WEBSOCKET_PEER_H

#include "core/error_list.h"
#include "core/io/packet_peer.h"
#include "websocket_macros.h"

class WebSocketPeer : public PacketPeer {
	GDCLASS(WebSocketPeer, PacketPeer);
	GDCICLASS(WebSocketPeer);

public:
	enum WriteMode {
		WRITE_MODE_TEXT,
		WRITE_MODE_BINARY,
	};

protected:
	static void _bind_methods();

public:
	virtual int get_available_packet_count() const = 0;
	virtual Error get_packet(const uint8_t **r_buffer, int &r_buffer_size) = 0;
	virtual Error put_packet(const uint8_t *p_buffer, int p_buffer_size) = 0;
	virtual int get_max_packet_size() const = 0;
	virtual int get_current_outbound_buffered_amount() const = 0;

	virtual WriteMode get_write_mode() const = 0;
	virtual void set_write_mode(WriteMode p_mode) = 0;

	virtual void close(int p_code = 1000, String p_reason = "") = 0;

	virtual bool is_connected_to_host() const = 0;
	virtual IP_Address get_connected_host() const = 0;
	virtual uint16_t get_connected_port() const = 0;
	virtual bool was_string_packet() const = 0;
	virtual void set_no_delay(bool p_enabled) = 0;

	WebSocketPeer();
	~WebSocketPeer();
};

VARIANT_ENUM_CAST(WebSocketPeer::WriteMode);

#endif // WEBSOCKET_PEER_H
