/**************************************************************************/
/*  audio_effect_compressor.h                                             */
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

#ifndef AUDIO_EFFECT_COMPRESSOR_H
#define AUDIO_EFFECT_COMPRESSOR_H

#include "servers/audio/audio_effect.h"

class AudioEffectCompressor;

class AudioEffectCompressorInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectCompressorInstance, AudioEffectInstance);
	friend class AudioEffectCompressor;
	Ref<AudioEffectCompressor> base;

	float rundb, averatio, runratio, runmax, maxover, gr_meter;
	int current_channel;

public:
	void set_current_channel(int p_channel) { current_channel = p_channel; }
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);
};

class AudioEffectCompressor : public AudioEffect {
	GDCLASS(AudioEffectCompressor, AudioEffect);

	friend class AudioEffectCompressorInstance;
	float threshold;
	float ratio;
	float gain;
	float attack_us;
	float release_ms;
	float mix;
	StringName sidechain;

protected:
	void _validate_property(PropertyInfo &property) const;
	static void _bind_methods();

public:
	Ref<AudioEffectInstance> instance();

	void set_threshold(float p_threshold);
	float get_threshold() const;

	void set_ratio(float p_ratio);
	float get_ratio() const;

	void set_gain(float p_gain);
	float get_gain() const;

	void set_attack_us(float p_attack_us);
	float get_attack_us() const;

	void set_release_ms(float p_release_ms);
	float get_release_ms() const;

	void set_mix(float p_mix);
	float get_mix() const;

	void set_sidechain(const StringName &p_sidechain);
	StringName get_sidechain() const;

	AudioEffectCompressor();
};

#endif // AUDIO_EFFECT_COMPRESSOR_H
