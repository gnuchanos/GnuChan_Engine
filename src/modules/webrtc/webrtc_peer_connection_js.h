/**************************************************************************/
/*  webrtc_peer_connection_js.h                                           */
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

#ifndef WEBRTC_PEER_CONNECTION_JS_H
#define WEBRTC_PEER_CONNECTION_JS_H

#ifdef JAVASCRIPT_ENABLED

#include "webrtc_peer_connection.h"

extern "C" {
typedef void (*RTCOnIceConnectionStateChange)(void *p_obj, int p_state);
typedef void (*RTCOnIceCandidate)(void *p_obj, const char *p_mid, int p_mline_idx, const char *p_candidate);
typedef void (*RTCOnDataChannel)(void *p_obj, int p_id);
typedef void (*RTCOnSession)(void *p_obj, const char *p_type, const char *p_sdp);
typedef void (*RTCOnError)(void *p_obj);
extern int godot_js_rtc_pc_create(const char *p_config, void *p_obj, RTCOnIceConnectionStateChange p_on_state_change, RTCOnIceCandidate p_on_candidate, RTCOnDataChannel p_on_datachannel);
extern void godot_js_rtc_pc_close(int p_id);
extern void godot_js_rtc_pc_destroy(int p_id);
extern void godot_js_rtc_pc_offer_create(int p_id, void *p_obj, RTCOnSession p_on_session, RTCOnError p_on_error);
extern void godot_js_rtc_pc_local_description_set(int p_id, const char *p_type, const char *p_sdp, void *p_obj, RTCOnError p_on_error);
extern void godot_js_rtc_pc_remote_description_set(int p_id, const char *p_type, const char *p_sdp, void *p_obj, RTCOnSession p_on_session, RTCOnError p_on_error);
extern void godot_js_rtc_pc_ice_candidate_add(int p_id, const char *p_mid_name, int p_mline_idx, const char *p_sdo);
extern int godot_js_rtc_pc_datachannel_create(int p_id, const char *p_label, const char *p_config);
}

class WebRTCPeerConnectionJS : public WebRTCPeerConnection {
	GDCLASS(WebRTCPeerConnectionJS, WebRTCPeerConnection);

private:
	int _js_id;
	ConnectionState _conn_state;

	static void _on_connection_state_changed(void *p_obj, int p_state);
	static void _on_ice_candidate(void *p_obj, const char *p_mid_name, int p_mline_idx, const char *p_candidate);
	static void _on_data_channel(void *p_obj, int p_channel);
	static void _on_session_created(void *p_obj, const char *p_type, const char *p_session);
	static void _on_error(void *p_obj);

public:
	static WebRTCPeerConnection *_create() { return memnew(WebRTCPeerConnectionJS); }
	static void make_default() { WebRTCPeerConnection::_create = WebRTCPeerConnectionJS::_create; }

	virtual ConnectionState get_connection_state() const;

	virtual Error initialize(Dictionary configuration = Dictionary());
	virtual Ref<WebRTCDataChannel> create_data_channel(String p_channel_name, Dictionary p_channel_config = Dictionary());
	virtual Error create_offer();
	virtual Error set_remote_description(String type, String sdp);
	virtual Error set_local_description(String type, String sdp);
	virtual Error add_ice_candidate(String sdpMidName, int sdpMlineIndexName, String sdpName);
	virtual Error poll();
	virtual void close();

	WebRTCPeerConnectionJS();
	~WebRTCPeerConnectionJS();
};

#endif

#endif // WEBRTC_PEER_CONNECTION_JS_H
