/**************************************************************************/
/*  audio_effect_amplify.cpp                                              */
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

#include "audio_effect_amplify.h"

void AudioEffectAmplifyInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {
	//multiply volume interpolating to avoid clicks if this changes
	float volume_db = base->volume_db;
	float vol = Math::db2linear(mix_volume_db);
	float vol_inc = (Math::db2linear(volume_db) - vol) / float(p_frame_count);

	for (int i = 0; i < p_frame_count; i++) {
		p_dst_frames[i] = p_src_frames[i] * vol;
		vol += vol_inc;
	}
	//set volume for next mix
	mix_volume_db = volume_db;
}

Ref<AudioEffectInstance> AudioEffectAmplify::instance() {
	Ref<AudioEffectAmplifyInstance> ins;
	ins.instance();
	ins->base = Ref<AudioEffectAmplify>(this);
	ins->mix_volume_db = volume_db;
	return ins;
}

void AudioEffectAmplify::set_volume_db(float p_volume) {
	volume_db = p_volume;
}

float AudioEffectAmplify::get_volume_db() const {
	return volume_db;
}

void AudioEffectAmplify::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_volume_db", "volume"), &AudioEffectAmplify::set_volume_db);
	ClassDB::bind_method(D_METHOD("get_volume_db"), &AudioEffectAmplify::get_volume_db);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "volume_db", PROPERTY_HINT_RANGE, "-80,24,0.01"), "set_volume_db", "get_volume_db");
}

AudioEffectAmplify::AudioEffectAmplify() {
	volume_db = 0;
}
