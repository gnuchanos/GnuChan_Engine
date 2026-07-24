/**************************************************************************/
/*  webrtc_multiplayer.h                                                  */
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

#ifndef WEBRTC_MULTIPLAYER_H
#define WEBRTC_MULTIPLAYER_H

#include "core/io/networked_multiplayer_peer.h"
#include "webrtc_peer_connection.h"

class WebRTCMultiplayer : public NetworkedMultiplayerPeer {
	GDCLASS(WebRTCMultiplayer, NetworkedMultiplayerPeer);

protected:
	static void _bind_methods();

private:
	enum {
		CH_RELIABLE = 0,
		CH_ORDERED = 1,
		CH_UNRELIABLE = 2,
		CH_RESERVED_MAX = 3
	};

	class ConnectedPeer : public Reference {
	public:
		Ref<WebRTCPeerConnection> connection;
		List<Ref<WebRTCDataChannel>> channels;
		bool connected;

		ConnectedPeer() {
			connected = false;
			for (int i = 0; i < CH_RESERVED_MAX; i++) {
				channels.push_front(Ref<WebRTCDataChannel>());
			}
		}
	};

	uint32_t unique_id;
	int target_peer;
	int client_count;
	bool refuse_connections;
	ConnectionStatus connection_status;
	TransferMode transfer_mode;
	int next_packet_peer;
	bool server_compat;

	Map<int, Ref<ConnectedPeer>> peer_map;

	void _peer_to_dict(Ref<ConnectedPeer> p_connected_peer, Dictionary &r_dict);
	void _find_next_peer();

public:
	WebRTCMultiplayer();
	~WebRTCMultiplayer();

	Error initialize(int p_self_id, bool p_server_compat = false);
	Error add_peer(Ref<WebRTCPeerConnection> p_peer, int p_peer_id, int p_unreliable_lifetime = 1);
	void remove_peer(int p_peer_id);
	bool has_peer(int p_peer_id);
	Dictionary get_peer(int p_peer_id);
	Dictionary get_peers();
	void close();

	// PacketPeer
	Error get_packet(const uint8_t **r_buffer, int &r_buffer_size); ///< buffer is GONE after next get_packet
	Error put_packet(const uint8_t *p_buffer, int p_buffer_size);
	int get_available_packet_count() const;
	int get_max_packet_size() const;

	// NetworkedMultiplayerPeer
	void set_transfer_mode(TransferMode p_mode);
	TransferMode get_transfer_mode() const;
	void set_target_peer(int p_peer_id);

	int get_unique_id() const;
	int get_packet_peer() const;

	bool is_server() const;

	void poll();

	void set_refuse_new_connections(bool p_enable);
	bool is_refusing_new_connections() const;

	ConnectionStatus get_connection_status() const;
};

#endif // WEBRTC_MULTIPLAYER_H
