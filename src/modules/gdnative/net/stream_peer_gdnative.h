/**************************************************************************/
/*  stream_peer_gdnative.h                                                */
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

#ifndef STREAM_PEER_GDNATIVE_H
#define STREAM_PEER_GDNATIVE_H

#include "core/io/stream_peer.h"
#include "modules/gdnative/gdnative.h"
#include "modules/gdnative/include/net/godot_net.h"

class StreamPeerGDNative : public StreamPeer {
	GDCLASS(StreamPeerGDNative, StreamPeer);

protected:
	static void _bind_methods();
	const godot_net_stream_peer *interface;

public:
	StreamPeerGDNative();
	~StreamPeerGDNative();

	/* Sets the interface implementation from GDNative */
	void set_native_stream_peer(const godot_net_stream_peer *p_interface);

	/* Specific to StreamPeer */
	Error put_data(const uint8_t *p_data, int p_bytes);
	Error put_partial_data(const uint8_t *p_data, int p_bytes, int &r_sent);
	Error get_data(uint8_t *p_buffer, int p_bytes);
	Error get_partial_data(uint8_t *p_buffer, int p_bytes, int &r_received);
	int get_available_bytes() const;
};

#endif // STREAM_PEER_GDNATIVE_H
