/**************************************************************************/
/*  audio_stream_generator.h                                              */
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

#ifndef AUDIO_STREAM_GENERATOR_H
#define AUDIO_STREAM_GENERATOR_H

#include "core/ring_buffer.h"
#include "servers/audio/audio_stream.h"

class AudioStreamGenerator : public AudioStream {
	GDCLASS(AudioStreamGenerator, AudioStream);

	float mix_rate;
	float buffer_len;

protected:
	static void _bind_methods();

public:
	void set_mix_rate(float p_mix_rate);
	float get_mix_rate() const;

	void set_buffer_length(float p_seconds);
	float get_buffer_length() const;

	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;

	virtual float get_length() const;
	AudioStreamGenerator();
};

class AudioStreamGeneratorPlayback : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamGeneratorPlayback, AudioStreamPlaybackResampled);
	friend class AudioStreamGenerator;
	RingBuffer<AudioFrame> buffer;
	int skips;
	bool active;
	float mixed;
	AudioStreamGenerator *generator;

protected:
	virtual void _mix_internal(AudioFrame *p_buffer, int p_frames);
	virtual float get_stream_sampling_rate();

	static void _bind_methods();

public:
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;

	virtual int get_loop_count() const; //times it looped

	virtual float get_playback_position() const;
	virtual void seek(float p_time);

	bool push_frame(const Vector2 &p_frame);
	bool can_push_buffer(int p_frames) const;
	bool push_buffer(const PoolVector2Array &p_frames);
	int get_frames_available() const;
	int get_skips() const;

	void clear_buffer();

	AudioStreamGeneratorPlayback();
};

#endif // AUDIO_STREAM_GENERATOR_H
