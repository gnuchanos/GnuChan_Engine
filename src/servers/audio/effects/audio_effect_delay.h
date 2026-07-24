/**************************************************************************/
/*  audio_effect_delay.h                                                  */
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

#ifndef AUDIO_EFFECT_DELAY_H
#define AUDIO_EFFECT_DELAY_H

#include "servers/audio/audio_effect.h"

class AudioEffectDelay;

class AudioEffectDelayInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectDelayInstance, AudioEffectInstance);
	friend class AudioEffectDelay;
	Ref<AudioEffectDelay> base;

	Vector<AudioFrame> ring_buffer;

	unsigned int ring_buffer_pos;
	unsigned int ring_buffer_mask;

	/* feedback buffer */
	Vector<AudioFrame> feedback_buffer;

	unsigned int feedback_buffer_pos;

	AudioFrame h;
	void _process_chunk(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);
};

class AudioEffectDelay : public AudioEffect {
	GDCLASS(AudioEffectDelay, AudioEffect);

	friend class AudioEffectDelayInstance;
	enum {

		MAX_DELAY_MS = 3000,
		MAX_TAPS = 2
	};

	float dry;

	bool tap_1_active;
	float tap_1_delay_ms;
	float tap_1_level;
	float tap_1_pan;

	bool tap_2_active;
	float tap_2_delay_ms;
	float tap_2_level;
	float tap_2_pan;

	bool feedback_active;
	float feedback_delay_ms;
	float feedback_level;
	float feedback_lowpass;

protected:
	static void _bind_methods();

public:
	void set_dry(float p_dry);
	float get_dry();

	void set_tap1_active(bool p_active);
	bool is_tap1_active() const;

	void set_tap1_delay_ms(float p_delay_ms);
	float get_tap1_delay_ms() const;

	void set_tap1_level_db(float p_level_db);
	float get_tap1_level_db() const;

	void set_tap1_pan(float p_pan);
	float get_tap1_pan() const;

	void set_tap2_active(bool p_active);
	bool is_tap2_active() const;

	void set_tap2_delay_ms(float p_delay_ms);
	float get_tap2_delay_ms() const;

	void set_tap2_level_db(float p_level_db);
	float get_tap2_level_db() const;

	void set_tap2_pan(float p_pan);
	float get_tap2_pan() const;

	void set_feedback_active(bool p_active);
	bool is_feedback_active() const;

	void set_feedback_delay_ms(float p_delay_ms);
	float get_feedback_delay_ms() const;

	void set_feedback_level_db(float p_level_db);
	float get_feedback_level_db() const;

	void set_feedback_lowpass(float p_lowpass);
	float get_feedback_lowpass() const;

	Ref<AudioEffectInstance> instance();

	AudioEffectDelay();
};

#endif // AUDIO_EFFECT_DELAY_H
