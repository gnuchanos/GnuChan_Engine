/**************************************************************************/
/*  video_stream_webm.h                                                   */
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

#ifndef VIDEO_STREAM_WEBM_H
#define VIDEO_STREAM_WEBM_H

#include "core/io/resource_loader.h"
#include "scene/resources/video_stream.h"

class WebMFrame;
class WebMDemuxer;
class VPXDecoder;
class OpusVorbisDecoder;

class VideoStreamPlaybackWebm : public VideoStreamPlayback {
	GDCLASS(VideoStreamPlaybackWebm, VideoStreamPlayback);

	String file_name;
	int audio_track;

	WebMDemuxer *webm;
	VPXDecoder *video;
	OpusVorbisDecoder *audio;

	WebMFrame **video_frames, *audio_frame;
	int video_frames_pos, video_frames_capacity;

	int num_decoded_samples, samples_offset;
	AudioMixCallback mix_callback;
	void *mix_udata;

	bool playing, paused;
	double delay_compensation;
	double time, video_frame_delay, video_pos;

	PoolVector<uint8_t> frame_data;
	Ref<ImageTexture> texture;

	float *pcm;

public:
	VideoStreamPlaybackWebm();
	~VideoStreamPlaybackWebm();

	bool open_file(const String &p_file);

	virtual void stop();
	virtual void play();

	virtual bool is_playing() const;

	virtual void set_paused(bool p_paused);
	virtual bool is_paused() const;

	virtual void set_loop(bool p_enable);
	virtual bool has_loop() const;

	virtual float get_length() const;

	virtual float get_playback_position() const;
	virtual void seek(float p_time);

	virtual void set_audio_track(int p_idx);

	virtual Ref<Texture> get_texture() const;
	virtual void update(float p_delta);

	virtual void set_mix_callback(AudioMixCallback p_callback, void *p_userdata);
	virtual int get_channels() const;
	virtual int get_mix_rate() const;

private:
	inline bool has_enough_video_frames() const;
	bool should_process(WebMFrame &video_frame);

	void delete_pointers();
};

/**/

class VideoStreamWebm : public VideoStream {
	GDCLASS(VideoStreamWebm, VideoStream);

	String file;
	int audio_track;

protected:
	static void _bind_methods();

public:
	VideoStreamWebm();

	virtual Ref<VideoStreamPlayback> instance_playback();

	virtual void set_file(const String &p_file);
	String get_file();
	virtual void set_audio_track(int p_track);
};

class ResourceFormatLoaderWebm : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

#endif // VIDEO_STREAM_WEBM_H
