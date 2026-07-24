/**************************************************************************/
/*  audio_effect_stereo_enhance.h                                         */
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

#ifndef AUDIO_EFFECT_STEREO_ENHANCE_H
#define AUDIO_EFFECT_STEREO_ENHANCE_H

#include "servers/audio/audio_effect.h"

class AudioEffectStereoEnhance;

class AudioEffectStereoEnhanceInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectStereoEnhanceInstance, AudioEffectInstance);
	friend class AudioEffectStereoEnhance;
	Ref<AudioEffectStereoEnhance> base;

	enum {

		MAX_DELAY_MS = 50
	};

	float *delay_ringbuff;
	unsigned int ringbuff_pos;
	unsigned int ringbuff_mask;

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);

	~AudioEffectStereoEnhanceInstance();
};

class AudioEffectStereoEnhance : public AudioEffect {
	GDCLASS(AudioEffectStereoEnhance, AudioEffect);

	friend class AudioEffectStereoEnhanceInstance;
	float volume_db;

	float pan_pullout;
	float time_pullout;
	float surround;

protected:
	static void _bind_methods();

public:
	Ref<AudioEffectInstance> instance();

	void set_pan_pullout(float p_amount);
	float get_pan_pullout() const;

	void set_time_pullout(float p_amount);
	float get_time_pullout() const;

	void set_surround(float p_amount);
	float get_surround() const;

	AudioEffectStereoEnhance();
};

#endif // AUDIO_EFFECT_STEREO_ENHANCE_H
