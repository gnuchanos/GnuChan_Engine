/**************************************************************************/
/*  audio_effect.h                                                        */
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

#ifndef AUDIO_EFFECT_H
#define AUDIO_EFFECT_H

#include "core/math/audio_frame.h"
#include "core/resource.h"

class AudioEffectInstance : public Reference {
	GDCLASS(AudioEffectInstance, Reference);

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count) = 0;
	virtual bool process_silence() const { return false; }
};

class AudioEffect : public Resource {
	GDCLASS(AudioEffect, Resource);

public:
	virtual Ref<AudioEffectInstance> instance() = 0;
	AudioEffect();
};

#endif // AUDIO_EFFECT_H
