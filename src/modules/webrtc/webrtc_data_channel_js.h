/**************************************************************************/
/*  webrtc_data_channel_js.h                                              */
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

#ifndef WEBRTC_DATA_CHANNEL_JS_H
#define WEBRTC_DATA_CHANNEL_JS_H

#ifdef JAVASCRIPT_ENABLED

#include "webrtc_data_channel.h"

class WebRTCDataChannelJS : public WebRTCDataChannel {
	GDCLASS(WebRTCDataChannelJS, WebRTCDataChannel);

private:
	String _label;
	String _protocol;

	bool _was_string;
	WriteMode _write_mode;

	enum {
		PACKET_BUFFER_SIZE = 65536 - 5 // 4 bytes for the size, 1 for for type
	};

	int _js_id;
	RingBuffer<uint8_t> in_buffer;
	int queue_count;
	uint8_t packet_buffer[PACKET_BUFFER_SIZE];

	static void _on_open(void *p_obj);
	static void _on_close(void *p_obj);
	static void _on_error(void *p_obj);
	static void _on_message(void *p_obj, const uint8_t *p_data, int p_size, int p_is_string);

public:
	virtual void set_write_mode(WriteMode mode);
	virtual WriteMode get_write_mode() const;
	virtual bool was_string_packet() const;

	virtual ChannelState get_ready_state() const;
	virtual String get_label() const;
	virtual bool is_ordered() const;
	virtual int get_id() const;
	virtual int get_max_packet_life_time() const;
	virtual int get_max_retransmits() const;
	virtual String get_protocol() const;
	virtual bool is_negotiated() const;
	virtual int get_buffered_amount() const;

	virtual Error poll();
	virtual void close();

	/** Inherited from PacketPeer: **/
	virtual int get_available_packet_count() const;
	virtual Error get_packet(const uint8_t **r_buffer, int &r_buffer_size); ///< buffer is GONE after next get_packet
	virtual Error put_packet(const uint8_t *p_buffer, int p_buffer_size);

	virtual int get_max_packet_size() const;

	WebRTCDataChannelJS();
	WebRTCDataChannelJS(int js_id);
	~WebRTCDataChannelJS();
};

#endif // JAVASCRIPT_ENABLED

#endif // WEBRTC_DATA_CHANNEL_JS_H
