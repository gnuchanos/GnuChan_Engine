/**************************************************************************/
/*  audio_effect_limiter.h                                                */
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

#ifndef AUDIO_EFFECT_LIMITER_H
#define AUDIO_EFFECT_LIMITER_H

#include "servers/audio/audio_effect.h"

class AudioEffectLimiter;

class AudioEffectLimiterInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectLimiterInstance, AudioEffectInstance);
	friend class AudioEffectLimiter;
	Ref<AudioEffectLimiter> base;

	float mix_volume_db;

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);
};

class AudioEffectLimiter : public AudioEffect {
	GDCLASS(AudioEffectLimiter, AudioEffect);

	friend class AudioEffectLimiterInstance;
	float threshold;
	float ceiling;
	float soft_clip;
	float soft_clip_ratio;

protected:
	static void _bind_methods();

public:
	void set_threshold_db(float p_threshold);
	float get_threshold_db() const;

	void set_ceiling_db(float p_ceiling);
	float get_ceiling_db() const;

	void set_soft_clip_db(float p_soft_clip);
	float get_soft_clip_db() const;

	void set_soft_clip_ratio(float p_soft_clip);
	float get_soft_clip_ratio() const;

	Ref<AudioEffectInstance> instance();

	AudioEffectLimiter();
};

#endif // AUDIO_EFFECT_LIMITER_H
