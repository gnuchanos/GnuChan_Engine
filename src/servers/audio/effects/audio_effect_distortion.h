/**************************************************************************/
/*  audio_effect_distortion.h                                             */
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

#ifndef AUDIO_EFFECT_DISTORTION_H
#define AUDIO_EFFECT_DISTORTION_H

#include "servers/audio/audio_effect.h"

class AudioEffectDistortion;

class AudioEffectDistortionInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectDistortionInstance, AudioEffectInstance);
	friend class AudioEffectDistortion;
	Ref<AudioEffectDistortion> base;
	float h[2];

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);
};

class AudioEffectDistortion : public AudioEffect {
	GDCLASS(AudioEffectDistortion, AudioEffect);

public:
	enum Mode {
		MODE_CLIP,
		MODE_ATAN,
		MODE_LOFI,
		MODE_OVERDRIVE,
		MODE_WAVESHAPE,
	};

	friend class AudioEffectDistortionInstance;
	Mode mode;
	float pre_gain;
	float post_gain;
	float keep_hf_hz;
	float drive;

protected:
	static void _bind_methods();

public:
	Ref<AudioEffectInstance> instance();

	void set_mode(Mode p_mode);
	Mode get_mode() const;

	void set_pre_gain(float p_pre_gain);
	float get_pre_gain() const;

	void set_keep_hf_hz(float p_keep_hf_hz);
	float get_keep_hf_hz() const;

	void set_drive(float p_drive);
	float get_drive() const;

	void set_post_gain(float p_post_gain);
	float get_post_gain() const;

	AudioEffectDistortion();
};

VARIANT_ENUM_CAST(AudioEffectDistortion::Mode)

#endif // AUDIO_EFFECT_DISTORTION_H
