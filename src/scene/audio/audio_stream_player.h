/**************************************************************************/
/*  audio_stream_player.h                                                 */
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

#ifndef AUDIO_STREAM_PLAYER_H
#define AUDIO_STREAM_PLAYER_H

#include "core/safe_refcount.h"
#include "scene/main/node.h"
#include "servers/audio/audio_stream.h"

class AudioStreamPlayer : public Node {
	GDCLASS(AudioStreamPlayer, Node);

public:
	enum MixTarget {
		MIX_TARGET_STEREO,
		MIX_TARGET_SURROUND,
		MIX_TARGET_CENTER
	};

private:
	Ref<AudioStreamPlayback> stream_playback;
	Ref<AudioStream> stream;
	Vector<AudioFrame> mix_buffer;
	Vector<AudioFrame> fadeout_buffer;
	bool use_fadeout = false;

	SafeNumeric<float> setseek;
	SafeFlag active;
	SafeFlag setstop;
	SafeFlag stop_has_priority;

	float mix_volume_db;
	float pitch_scale;
	float volume_db;
	bool autoplay;
	bool stream_paused;
	bool stream_paused_fade;
	StringName bus;

	MixTarget mix_target;

	void _mix_internal(bool p_fadeout);
	void _mix_audio();
	static void _mix_audios(void *self) { reinterpret_cast<AudioStreamPlayer *>(self)->_mix_audio(); }

	void _set_playing(bool p_enable);
	bool _is_active() const;

	void _bus_layout_changed();
	void _mix_to_bus(const AudioFrame *p_frames, int p_amount);

protected:
	void _validate_property(PropertyInfo &property) const;
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_stream(Ref<AudioStream> p_stream);
	Ref<AudioStream> get_stream() const;

	void set_volume_db(float p_volume);
	float get_volume_db() const;

	void set_pitch_scale(float p_pitch_scale);
	float get_pitch_scale() const;

	void play(float p_from_pos = 0.0);
	void seek(float p_seconds);
	void stop();
	bool is_playing() const;
	float get_playback_position();

	void set_bus(const StringName &p_bus);
	StringName get_bus() const;

	void set_autoplay(bool p_enable);
	bool is_autoplay_enabled();

	void set_mix_target(MixTarget p_target);
	MixTarget get_mix_target() const;

	void set_stream_paused(bool p_pause);
	bool get_stream_paused() const;

	Ref<AudioStreamPlayback> get_stream_playback();

	AudioStreamPlayer();
	~AudioStreamPlayer();
};

VARIANT_ENUM_CAST(AudioStreamPlayer::MixTarget)

#endif // AUDIO_STREAM_PLAYER_H
