/**************************************************************************/
/*  webrtc_data_channel_gdnative.h                                        */
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

#ifndef WEBRTC_DATA_CHANNEL_GDNATIVE_H
#define WEBRTC_DATA_CHANNEL_GDNATIVE_H

#ifdef WEBRTC_GDNATIVE_ENABLED

#include "modules/gdnative/include/net/godot_net.h"
#include "webrtc_data_channel.h"

class WebRTCDataChannelGDNative : public WebRTCDataChannel {
	GDCLASS(WebRTCDataChannelGDNative, WebRTCDataChannel);

protected:
	static void _bind_methods();

private:
	const godot_net_webrtc_data_channel *interface;

public:
	void set_native_webrtc_data_channel(const godot_net_webrtc_data_channel *p_impl);

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

	WebRTCDataChannelGDNative();
	~WebRTCDataChannelGDNative();
};

#endif // WEBRTC_GDNATIVE_ENABLED

#endif // WEBRTC_DATA_CHANNEL_GDNATIVE_H
