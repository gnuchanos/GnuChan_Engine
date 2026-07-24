/**************************************************************************/
/*  networked_multiplayer_peer.h                                          */
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

#ifndef NETWORKED_MULTIPLAYER_PEER_H
#define NETWORKED_MULTIPLAYER_PEER_H

#include "core/io/packet_peer.h"

class NetworkedMultiplayerPeer : public PacketPeer {
	GDCLASS(NetworkedMultiplayerPeer, PacketPeer);

protected:
	static void _bind_methods();

public:
	enum {
		TARGET_PEER_BROADCAST = 0,
		TARGET_PEER_SERVER = 1
	};
	enum TransferMode {
		TRANSFER_MODE_UNRELIABLE,
		TRANSFER_MODE_UNRELIABLE_ORDERED,
		TRANSFER_MODE_RELIABLE,
	};

	enum ConnectionStatus {
		CONNECTION_DISCONNECTED,
		CONNECTION_CONNECTING,
		CONNECTION_CONNECTED,
	};

	virtual void set_transfer_mode(TransferMode p_mode) = 0;
	virtual TransferMode get_transfer_mode() const = 0;
	virtual void set_target_peer(int p_peer_id) = 0;

	virtual int get_packet_peer() const = 0;

	virtual bool is_server() const = 0;

	virtual void poll() = 0;

	virtual int get_unique_id() const = 0;

	virtual void set_refuse_new_connections(bool p_enable) = 0;
	virtual bool is_refusing_new_connections() const = 0;

	virtual ConnectionStatus get_connection_status() const = 0;

	NetworkedMultiplayerPeer();
};

VARIANT_ENUM_CAST(NetworkedMultiplayerPeer::TransferMode)
VARIANT_ENUM_CAST(NetworkedMultiplayerPeer::ConnectionStatus)

#endif // NETWORKED_MULTIPLAYER_PEER_H
