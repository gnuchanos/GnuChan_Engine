/**************************************************************************/
/*  midi_driver.h                                                         */
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

#ifndef MIDI_DRIVER_H
#define MIDI_DRIVER_H

#include "core/typedefs.h"
#include "core/variant.h"

/**
 * Multi-Platform abstraction for accessing to MIDI.
 */

class MIDIDriver {
	static MIDIDriver *singleton;
	static uint8_t last_received_message;

public:
	static MIDIDriver *get_singleton();
	void set_singleton();

	virtual Error open() = 0;
	virtual void close() = 0;

	virtual PoolStringArray get_connected_inputs();

	static void receive_input_packet(uint64_t timestamp, uint8_t *data, uint32_t length);

	MIDIDriver();
	virtual ~MIDIDriver() {}
};

#endif // MIDI_DRIVER_H
