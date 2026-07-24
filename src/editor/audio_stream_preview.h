/**************************************************************************/
/*  audio_stream_preview.h                                                */
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

#ifndef AUDIO_STREAM_PREVIEW_H
#define AUDIO_STREAM_PREVIEW_H

#include "core/os/thread.h"
#include "core/safe_refcount.h"
#include "scene/main/node.h"
#include "servers/audio/audio_stream.h"

class AudioStreamPreview : public Reference {
	GDCLASS(AudioStreamPreview, Reference);
	friend class AudioStream;
	Vector<uint8_t> preview;
	float length;

	friend class AudioStreamPreviewGenerator;

public:
	float get_length() const;
	float get_max(float p_time, float p_time_next) const;
	float get_min(float p_time, float p_time_next) const;

	AudioStreamPreview();
};

class AudioStreamPreviewGenerator : public Node {
	GDCLASS(AudioStreamPreviewGenerator, Node);

	static AudioStreamPreviewGenerator *singleton;

	struct Preview {
		Ref<AudioStreamPreview> preview;
		Ref<AudioStream> base_stream;
		Ref<AudioStreamPlayback> playback;
		SafeFlag generating;
		ObjectID id;
		Thread *thread;

		// Needed for the bookkeeping of the Map
		Preview &operator=(const Preview &p_rhs) {
			preview = p_rhs.preview;
			base_stream = p_rhs.base_stream;
			playback = p_rhs.playback;
			generating.set_to(generating.is_set());
			id = p_rhs.id;
			thread = p_rhs.thread;
			return *this;
		}
	};

	Map<ObjectID, Preview> previews;

	static void _preview_thread(void *p_preview);

	void _update_emit(ObjectID p_id);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	static AudioStreamPreviewGenerator *get_singleton() { return singleton; }

	Ref<AudioStreamPreview> generate_preview(const Ref<AudioStream> &p_stream);

	AudioStreamPreviewGenerator();
};

#endif // AUDIO_STREAM_PREVIEW_H
