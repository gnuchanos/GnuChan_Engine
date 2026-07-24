/**************************************************************************/
/*  godot_webrtc.h                                                        */
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

#ifndef GODOT_WEBRTC_H
#define GODOT_WEBRTC_H

#include <gdnative/gdnative.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GODOT_NET_WEBRTC_API_MAJOR 3
#define GODOT_NET_WEBRTC_API_MINOR 4

/* Library Interface (used to set default GDNative WebRTC implementation */
typedef struct {
	godot_gdnative_api_version version; /* version of our API */

	/* Called when the library is unset as default interface via godot_net_set_webrtc_library */
	void (*unregistered)();

	/* Used by WebRTCPeerConnection create when GDNative is the default implementation. */
	/* Takes a pointer to WebRTCPeerConnectionGDNative, should bind and return OK, failure if binding was unsuccessful. */
	godot_error (*create_peer_connection)(godot_object *);

	void *next; /* For extension */
} godot_net_webrtc_library;

/* WebRTCPeerConnection interface */
typedef struct {
	godot_gdnative_api_version version; /* version of our API */

	godot_object *data; /* User reference */

	/* This is WebRTCPeerConnection */
	godot_int (*get_connection_state)(const void *);

	godot_error (*initialize)(void *, const godot_dictionary *);
	godot_object *(*create_data_channel)(void *, const char *p_channel_name, const godot_dictionary *);
	godot_error (*create_offer)(void *);
	godot_error (*create_answer)(void *); /* unused for now, should be done automatically on set_local_description */
	godot_error (*set_remote_description)(void *, const char *, const char *);
	godot_error (*set_local_description)(void *, const char *, const char *);
	godot_error (*add_ice_candidate)(void *, const char *, int, const char *);
	godot_error (*poll)(void *);
	void (*close)(void *);

	void *next; /* For extension? */
} godot_net_webrtc_peer_connection;

/* WebRTCDataChannel interface */
typedef struct {
	godot_gdnative_api_version version; /* version of our API */

	godot_object *data; /* User reference */

	/* This is PacketPeer */
	godot_error (*get_packet)(void *, const uint8_t **, int *);
	godot_error (*put_packet)(void *, const uint8_t *, int);
	godot_int (*get_available_packet_count)(const void *);
	godot_int (*get_max_packet_size)(const void *);

	/* This is WebRTCDataChannel */
	void (*set_write_mode)(void *, godot_int);
	godot_int (*get_write_mode)(const void *);
	bool (*was_string_packet)(const void *);

	godot_int (*get_ready_state)(const void *);
	const char *(*get_label)(const void *);
	bool (*is_ordered)(const void *);
	int (*get_id)(const void *);
	int (*get_max_packet_life_time)(const void *);
	int (*get_max_retransmits)(const void *);
	const char *(*get_protocol)(const void *);
	bool (*is_negotiated)(const void *);

	godot_error (*poll)(void *);
	void (*close)(void *);

	void *next; /* For extension? */
} godot_net_webrtc_data_channel;

/* Extensions to WebRTCDataChannel */
typedef struct {
	int (*get_buffered_amount)(const void *);

	void *next; /* For extension? */
} godot_net_webrtc_data_channel_ext;

/* Set the default GDNative library */
godot_error GDAPI godot_net_set_webrtc_library(const godot_net_webrtc_library *);
/* Binds a WebRTCPeerConnectionGDNative to the provided interface */
void GDAPI godot_net_bind_webrtc_peer_connection(godot_object *p_obj, const godot_net_webrtc_peer_connection *);
/* Binds a WebRTCDataChannelGDNative to the provided interface */
void GDAPI godot_net_bind_webrtc_data_channel(godot_object *p_obj, const godot_net_webrtc_data_channel *);

#ifdef __cplusplus
}
#endif

#endif // GODOT_WEBRTC_H
