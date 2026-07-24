/**************************************************************************/
/*  midi_driver_winmidi.h                                                 */
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

#ifndef MIDI_DRIVER_WINMIDI_H
#define MIDI_DRIVER_WINMIDI_H

#ifdef WINMIDI_ENABLED

#include "core/os/midi_driver.h"
#include "core/vector.h"

#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmsystem.h>

class MIDIDriverWinMidi : public MIDIDriver {
	Vector<HMIDIIN> connected_sources;

	static void CALLBACK read(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

public:
	virtual Error open();
	virtual void close();

	virtual PoolStringArray get_connected_inputs();

	MIDIDriverWinMidi();
	virtual ~MIDIDriverWinMidi();
};

#endif // WINMIDI_ENABLED

#endif // MIDI_DRIVER_WINMIDI_H
