/**************************************************************************/
/*  audio_effect_capture.h                                                */
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

#ifndef AUDIO_EFFECT_CAPTURE_H
#define AUDIO_EFFECT_CAPTURE_H

#include "core/engine.h"
#include "core/math/audio_frame.h"
#include "core/pool_vector.h"
#include "core/reference.h"
#include "core/ring_buffer.h"
#include "servers/audio/audio_effect.h"
#include "servers/audio_server.h"

class AudioEffectCapture;

class AudioEffectCaptureInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectCaptureInstance, AudioEffectInstance);
	friend class AudioEffectCapture;
	Ref<AudioEffectCapture> base;

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);
	virtual bool process_silence() const;
};

class AudioEffectCapture : public AudioEffect {
	GDCLASS(AudioEffectCapture, AudioEffect)
	friend class AudioEffectCaptureInstance;

	RingBuffer<AudioFrame> buffer;
	uint64_t discarded_frames;
	uint64_t pushed_frames;
	float buffer_length_seconds;
	bool buffer_initialized;

protected:
	static void _bind_methods();

public:
	AudioEffectCapture() {
		discarded_frames = 0;
		pushed_frames = 0;
		buffer_length_seconds = 0.1f;
		buffer_initialized = false;
	}
	virtual Ref<AudioEffectInstance> instance();

	void set_buffer_length(float p_buffer_length_seconds);
	float get_buffer_length();

	bool can_get_buffer(int p_frames) const;
	PoolVector2Array get_buffer(int p_len);
	void clear_buffer();

	int get_frames_available() const;
	int64_t get_discarded_frames() const;
	int get_buffer_length_frames() const;
	int64_t get_pushed_frames() const;
};

#endif // AUDIO_EFFECT_CAPTURE_H
