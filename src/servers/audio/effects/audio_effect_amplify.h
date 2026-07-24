/**************************************************************************/
/*  audio_effect_amplify.h                                                */
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

#ifndef AUDIO_EFFECT_AMPLIFY_H
#define AUDIO_EFFECT_AMPLIFY_H

#include "servers/audio/audio_effect.h"

class AudioEffectAmplify;

class AudioEffectAmplifyInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectAmplifyInstance, AudioEffectInstance);
	friend class AudioEffectAmplify;
	Ref<AudioEffectAmplify> base;

	float mix_volume_db;

public:
	virtual void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);
};

class AudioEffectAmplify : public AudioEffect {
	GDCLASS(AudioEffectAmplify, AudioEffect);

	friend class AudioEffectAmplifyInstance;
	float volume_db;

protected:
	static void _bind_methods();

public:
	Ref<AudioEffectInstance> instance();
	void set_volume_db(float p_volume);
	float get_volume_db() const;

	AudioEffectAmplify();
};

#endif // AUDIO_EFFECT_AMPLIFY_H
