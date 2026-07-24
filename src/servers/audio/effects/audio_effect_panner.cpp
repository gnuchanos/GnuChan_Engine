/**************************************************************************/
/*  audio_effect_panner.cpp                                               */
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

#include "audio_effect_panner.h"

void AudioEffectPannerInstance::process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) {
	float lvol = CLAMP(1.0 - base->pan, 0, 1);
	float rvol = CLAMP(1.0 + base->pan, 0, 1);

	for (int i = 0; i < p_frame_count; i++) {
		p_dst_frames[i].l = p_src_frames[i].l * lvol + p_src_frames[i].r * (1.0 - rvol);
		p_dst_frames[i].r = p_src_frames[i].r * rvol + p_src_frames[i].l * (1.0 - lvol);
	}
}

Ref<AudioEffectInstance> AudioEffectPanner::instance() {
	Ref<AudioEffectPannerInstance> ins;
	ins.instance();
	ins->base = Ref<AudioEffectPanner>(this);
	return ins;
}

void AudioEffectPanner::set_pan(float p_cpanume) {
	pan = p_cpanume;
}

float AudioEffectPanner::get_pan() const {
	return pan;
}

void AudioEffectPanner::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_pan", "cpanume"), &AudioEffectPanner::set_pan);
	ClassDB::bind_method(D_METHOD("get_pan"), &AudioEffectPanner::get_pan);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "pan", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_pan", "get_pan");
}

AudioEffectPanner::AudioEffectPanner() {
	pan = 0;
}
