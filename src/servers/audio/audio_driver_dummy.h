/**************************************************************************/
/*  audio_driver_dummy.h                                                  */
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

#ifndef AUDIO_DRIVER_DUMMY_H
#define AUDIO_DRIVER_DUMMY_H

#include "servers/audio_server.h"

#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "core/safe_refcount.h"

class AudioDriverDummy : public AudioDriver {
	Thread thread;
	Mutex mutex;

	int32_t *samples_in;

	static void thread_func(void *p_udata);

	unsigned int buffer_frames;
	unsigned int mix_rate;
	SpeakerMode speaker_mode;

	int channels;

	SafeFlag active;
	SafeFlag exit_thread;

public:
	const char *get_name() const {
		return "Dummy";
	};

	virtual Error init();
	virtual void start();
	virtual int get_mix_rate() const;
	virtual SpeakerMode get_speaker_mode() const;
	virtual void lock();
	virtual void unlock();
	virtual void finish();

	AudioDriverDummy();
	~AudioDriverDummy();
};

#endif // AUDIO_DRIVER_DUMMY_H
