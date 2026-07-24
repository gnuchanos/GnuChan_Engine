/**************************************************************************/
/*  tts_windows.h                                                         */
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

#ifndef TTS_WINDOWS_H
#define TTS_WINDOWS_H

#include "core/array.h"
#include "core/list.h"
#include "core/map.h"
#include "core/os/os.h"
#include "core/ustring.h"

#include <objbase.h>
#include <sapi.h>
#include <wchar.h>
#include <winnls.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class TTS_Windows {
	List<OS::TTSUtterance> queue;
	ISpVoice *synth = nullptr;
	bool paused = false;
	struct UTData {
		String string;
		int offset;
		int id;
	};
	Map<ULONG, UTData> ids;

	static void __stdcall speech_event_callback(WPARAM wParam, LPARAM lParam);
	void _update_tts();

	static TTS_Windows *singleton;

public:
	static TTS_Windows *get_singleton();

	bool is_speaking() const;
	bool is_paused() const;
	Array get_voices() const;

	void speak(const String &p_text, const String &p_voice, int p_volume = 50, float p_pitch = 1.f, float p_rate = 1.f, int p_utterance_id = 0, bool p_interrupt = false);
	void pause();
	void resume();
	void stop();

	TTS_Windows();
	~TTS_Windows();
};

#endif // TTS_WINDOWS_H
