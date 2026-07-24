/**************************************************************************/
/*  tts_linux.h                                                           */
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

#ifndef TTS_LINUX_H
#define TTS_LINUX_H

#include "core/array.h"
#include "core/list.h"
#include "core/map.h"
#include "core/os/os.h"
#include "core/os/thread.h"
#include "core/os/thread_safe.h"
#include "core/ustring.h"

#include "speechd-so_wrap.h"

class TTS_Linux {
	_THREAD_SAFE_CLASS_

	List<OS::TTSUtterance> queue;
	SPDConnection *synth = nullptr;
	bool speaking = false;
	bool paused = false;
	int last_msg_id = -1;
	HashMap<int, int> ids;

	Thread init_thread;

	static void speech_init_thread_func(void *p_userdata);
	static void speech_event_callback(size_t p_msg_id, size_t p_client_id, SPDNotificationType p_type);
	static void speech_event_index_mark(size_t p_msg_id, size_t p_client_id, SPDNotificationType p_type, char *p_index_mark);

	static TTS_Linux *singleton;

public:
	static TTS_Linux *get_singleton();

	bool is_speaking() const;
	bool is_paused() const;
	Array get_voices() const;

	void speak(const String &p_text, const String &p_voice, int p_volume = 50, float p_pitch = 1.f, float p_rate = 1.f, int p_utterance_id = 0, bool p_interrupt = false);
	void pause();
	void resume();
	void stop();

	TTS_Linux();
	~TTS_Linux();
};

#endif // TTS_LINUX_H
