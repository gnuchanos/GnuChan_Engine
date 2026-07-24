/**************************************************************************/
/*  webrtc_data_channel.h                                                 */
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

#ifndef WEBRTC_DATA_CHANNEL_H
#define WEBRTC_DATA_CHANNEL_H

#include "core/io/packet_peer.h"

#define WRTC_IN_BUF PNAME("network/limits/webrtc/max_channel_in_buffer_kb")

class WebRTCDataChannel : public PacketPeer {
	GDCLASS(WebRTCDataChannel, PacketPeer);

public:
	enum WriteMode {
		WRITE_MODE_TEXT,
		WRITE_MODE_BINARY,
	};

	enum ChannelState {
		STATE_CONNECTING,
		STATE_OPEN,
		STATE_CLOSING,
		STATE_CLOSED
	};

protected:
	unsigned int _in_buffer_shift;

	static void _bind_methods();

public:
	virtual void set_write_mode(WriteMode mode) = 0;
	virtual WriteMode get_write_mode() const = 0;
	virtual bool was_string_packet() const = 0;

	virtual ChannelState get_ready_state() const = 0;
	virtual String get_label() const = 0;
	virtual bool is_ordered() const = 0;
	virtual int get_id() const = 0;
	virtual int get_max_packet_life_time() const = 0;
	virtual int get_max_retransmits() const = 0;
	virtual String get_protocol() const = 0;
	virtual bool is_negotiated() const = 0;

	virtual int get_buffered_amount() const = 0;

	virtual Error poll() = 0;
	virtual void close() = 0;

	/** Inherited from PacketPeer: **/
	virtual int get_available_packet_count() const = 0;
	virtual Error get_packet(const uint8_t **r_buffer, int &r_buffer_size) = 0; ///< buffer is GONE after next get_packet
	virtual Error put_packet(const uint8_t *p_buffer, int p_buffer_size) = 0;

	virtual int get_max_packet_size() const = 0;

	WebRTCDataChannel();
	~WebRTCDataChannel();
};

VARIANT_ENUM_CAST(WebRTCDataChannel::WriteMode);
VARIANT_ENUM_CAST(WebRTCDataChannel::ChannelState);

#endif // WEBRTC_DATA_CHANNEL_H
