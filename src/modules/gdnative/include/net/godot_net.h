/**************************************************************************/
/*  godot_net.h                                                           */
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

#ifndef GODOT_NET_H
#define GODOT_NET_H

#include <gdnative/gdnative.h>

#ifdef __cplusplus
extern "C" {
#endif

// For future versions of the API we should only add new functions at the end of the structure and use the
// version info to detect whether a call is available

// Use these to populate version in your plugin
#define GODOT_NET_API_MAJOR 3
#define GODOT_NET_API_MINOR 1

typedef struct {
	godot_gdnative_api_version version; /* version of our API */
	godot_object *data; /* User reference */

	/* This is StreamPeer */
	godot_error (*get_data)(void *user, uint8_t *p_buffer, int p_bytes);
	godot_error (*get_partial_data)(void *user, uint8_t *p_buffer, int p_bytes, int *r_received);
	godot_error (*put_data)(void *user, const uint8_t *p_data, int p_bytes);
	godot_error (*put_partial_data)(void *user, const uint8_t *p_data, int p_bytes, int *r_sent);

	int (*get_available_bytes)(const void *user);

	void *next; /* For extension? */
} godot_net_stream_peer;

/* Binds a StreamPeerGDNative to the provided interface */
void godot_net_bind_stream_peer(godot_object *p_obj, const godot_net_stream_peer *p_interface);

typedef struct {
	godot_gdnative_api_version version; /* version of our API */

	godot_object *data; /* User reference */

	/* This is PacketPeer */
	godot_error (*get_packet)(void *, const uint8_t **, int *);
	godot_error (*put_packet)(void *, const uint8_t *, int);
	godot_int (*get_available_packet_count)(const void *);
	godot_int (*get_max_packet_size)(const void *);

	void *next; /* For extension? */
} godot_net_packet_peer;

/* Binds a PacketPeerGDNative to the provided interface */
void GDAPI godot_net_bind_packet_peer(godot_object *p_obj, const godot_net_packet_peer *);

typedef struct {
	godot_gdnative_api_version version; /* version of our API */

	godot_object *data; /* User reference */

	/* This is PacketPeer */
	godot_error (*get_packet)(void *, const uint8_t **, int *);
	godot_error (*put_packet)(void *, const uint8_t *, int);
	godot_int (*get_available_packet_count)(const void *);
	godot_int (*get_max_packet_size)(const void *);

	/* This is NetworkedMultiplayerPeer */
	void (*set_transfer_mode)(void *, godot_int);
	godot_int (*get_transfer_mode)(const void *);
	// 0 = broadcast, 1 = server, <0 = all but abs(value)
	void (*set_target_peer)(void *, godot_int);
	godot_int (*get_packet_peer)(const void *);
	godot_bool (*is_server)(const void *);
	void (*poll)(void *);
	// Must be > 0, 1 is for server
	int32_t (*get_unique_id)(const void *);
	void (*set_refuse_new_connections)(void *, godot_bool);
	godot_bool (*is_refusing_new_connections)(const void *);
	godot_int (*get_connection_status)(const void *);

	void *next; /* For extension? Or maybe not... */
} godot_net_multiplayer_peer;

/* Binds a MultiplayerPeerGDNative to the provided interface */
void GDAPI godot_net_bind_multiplayer_peer(godot_object *p_obj, const godot_net_multiplayer_peer *);

#ifdef __cplusplus
}
#endif

// WebRTC Bindings
#include "net/godot_webrtc.h"

#endif // GODOT_NET_H
