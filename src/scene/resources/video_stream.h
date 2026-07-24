/**************************************************************************/
/*  video_stream.h                                                        */
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

#ifndef VIDEO_STREAM_H
#define VIDEO_STREAM_H

#include "scene/resources/texture.h"

class VideoStreamPlayback : public Resource {
	GDCLASS(VideoStreamPlayback, Resource);

public:
	typedef int (*AudioMixCallback)(void *p_udata, const float *p_data, int p_frames);

	virtual void stop() = 0;
	virtual void play() = 0;

	virtual bool is_playing() const = 0;

	virtual void set_paused(bool p_paused) = 0;
	virtual bool is_paused() const = 0;

	virtual void set_loop(bool p_enable) = 0;
	virtual bool has_loop() const = 0;

	virtual float get_length() const = 0;

	virtual float get_playback_position() const = 0;
	virtual void seek(float p_time) = 0;

	virtual void set_audio_track(int p_idx) = 0;

	virtual Ref<Texture> get_texture() const = 0;
	virtual void update(float p_delta) = 0;

	virtual void set_mix_callback(AudioMixCallback p_callback, void *p_userdata) = 0;
	virtual int get_channels() const = 0;
	virtual int get_mix_rate() const = 0;
};

class VideoStream : public Resource {
	GDCLASS(VideoStream, Resource);
	OBJ_SAVE_TYPE(VideoStream); // Saves derived classes with common type so they can be interchanged.

public:
	virtual void set_audio_track(int p_track) = 0;
	virtual Ref<VideoStreamPlayback> instance_playback() = 0;
};

#endif // VIDEO_STREAM_H
