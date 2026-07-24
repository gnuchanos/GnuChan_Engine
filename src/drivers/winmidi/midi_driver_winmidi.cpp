/**************************************************************************/
/*  midi_driver_winmidi.cpp                                               */
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

#ifdef WINMIDI_ENABLED

#include "midi_driver_winmidi.h"

#include "core/print_string.h"

void MIDIDriverWinMidi::read(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (wMsg == MIM_DATA) {
		receive_input_packet((uint64_t)dwParam2, (uint8_t *)&dwParam1, 3);
	}
}

Error MIDIDriverWinMidi::open() {
	for (UINT i = 0; i < midiInGetNumDevs(); i++) {
		HMIDIIN midi_in;

		MMRESULT res = midiInOpen(&midi_in, i, (DWORD_PTR)read, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if (res == MMSYSERR_NOERROR) {
			midiInStart(midi_in);
			connected_sources.insert(i, midi_in);
		} else {
			char err[256];
			midiInGetErrorText(res, err, 256);
			ERR_PRINT("midiInOpen error: " + String(err));

			MIDIINCAPS caps;
			res = midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
			if (res == MMSYSERR_NOERROR) {
				ERR_PRINT("Can't open MIDI device \"" + String(caps.szPname) + "\", is it being used by another application?");
			}
		}
	}

	return OK;
}

PoolStringArray MIDIDriverWinMidi::get_connected_inputs() {
	PoolStringArray list;

	for (int i = 0; i < connected_sources.size(); i++) {
		HMIDIIN midi_in = connected_sources[i];
		UINT id = 0;
		MMRESULT res = midiInGetID(midi_in, &id);
		if (res == MMSYSERR_NOERROR) {
			MIDIINCAPS caps;
			res = midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS));
			if (res == MMSYSERR_NOERROR) {
				list.push_back(caps.szPname);
			}
		}
	}

	return list;
}

void MIDIDriverWinMidi::close() {
	for (int i = 0; i < connected_sources.size(); i++) {
		HMIDIIN midi_in = connected_sources[i];
		midiInStop(midi_in);
		midiInClose(midi_in);
	}
	connected_sources.clear();
}

MIDIDriverWinMidi::MIDIDriverWinMidi() {
}

MIDIDriverWinMidi::~MIDIDriverWinMidi() {
	close();
}

#endif // WINMIDI_ENABLED
