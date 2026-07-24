/**************************************************************************/
/*  multiplayer_peer_gdnative.h                                           */
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

#ifndef MULTIPLAYER_PEER_GDNATIVE_H
#define MULTIPLAYER_PEER_GDNATIVE_H

#include "core/io/networked_multiplayer_peer.h"
#include "modules/gdnative/gdnative.h"
#include "modules/gdnative/include/net/godot_net.h"

class MultiplayerPeerGDNative : public NetworkedMultiplayerPeer {
	GDCLASS(MultiplayerPeerGDNative, NetworkedMultiplayerPeer);

protected:
	static void _bind_methods();
	const godot_net_multiplayer_peer *interface;

public:
	MultiplayerPeerGDNative();
	~MultiplayerPeerGDNative();

	/* Sets the interface implementation from GDNative */
	void set_native_multiplayer_peer(const godot_net_multiplayer_peer *p_impl);

	/* Specific to PacketPeer */
	virtual Error get_packet(const uint8_t **r_buffer, int &r_buffer_size);
	virtual Error put_packet(const uint8_t *p_buffer, int p_buffer_size);
	virtual int get_max_packet_size() const;
	virtual int get_available_packet_count() const;

	/* Specific to NetworkedMultiplayerPeer */
	virtual void set_transfer_mode(TransferMode p_mode);
	virtual TransferMode get_transfer_mode() const;
	virtual void set_target_peer(int p_peer_id);

	virtual int get_packet_peer() const;

	virtual bool is_server() const;

	virtual void poll();

	virtual int get_unique_id() const;

	virtual void set_refuse_new_connections(bool p_enable);
	virtual bool is_refusing_new_connections() const;

	virtual ConnectionStatus get_connection_status() const;
};

#endif // MULTIPLAYER_PEER_GDNATIVE_H
