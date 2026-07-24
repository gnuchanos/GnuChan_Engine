/**************************************************************************/
/*  audio_driver_xaudio2.h                                                */
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

#ifndef AUDIO_DRIVER_XAUDIO2_H
#define AUDIO_DRIVER_XAUDIO2_H

#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "core/safe_refcount.h"
#include "servers/audio_server.h"

#include <mmsystem.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl/client.h>
#include <xaudio2.h>

class AudioDriverXAudio2 : public AudioDriver {
	enum {
		AUDIO_BUFFERS = 2
	};

	struct XAudio2DriverVoiceCallback : public IXAudio2VoiceCallback {
		HANDLE buffer_end_event;
		XAudio2DriverVoiceCallback() :
				buffer_end_event(CreateEvent(NULL, FALSE, FALSE, NULL)) {}
		void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) {
			SetEvent(buffer_end_event);
		}

		//Unused methods are stubs
		void STDMETHODCALLTYPE OnStreamEnd() {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
		void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) {}
		void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) {}
		void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, HRESULT Error) {}
	};

	Thread thread;
	Mutex mutex;

	int32_t *samples_in;
	int16_t *samples_out[AUDIO_BUFFERS];

	static void thread_func(void *p_udata);
	int buffer_size;

	unsigned int mix_rate;
	SpeakerMode speaker_mode;

	int channels;

	SafeFlag active;
	SafeFlag exit_thread;
	bool pcm_open;

	WAVEFORMATEX wave_format;
	Microsoft::WRL::ComPtr<IXAudio2> xaudio;
	int current_buffer;
	IXAudio2MasteringVoice *mastering_voice;
	XAUDIO2_BUFFER xaudio_buffer[AUDIO_BUFFERS];
	IXAudio2SourceVoice *source_voice;
	XAudio2DriverVoiceCallback voice_callback;

public:
	const char *get_name() const;

	virtual Error init();
	virtual void start();
	virtual int get_mix_rate() const;
	virtual SpeakerMode get_speaker_mode() const;
	virtual float get_latency();
	virtual void lock();
	virtual void unlock();
	virtual void finish();

	AudioDriverXAudio2();
	~AudioDriverXAudio2();
};

#endif // AUDIO_DRIVER_XAUDIO2_H
