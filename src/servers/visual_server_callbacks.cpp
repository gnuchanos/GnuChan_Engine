/**************************************************************************/
/*  visual_server_callbacks.cpp                                           */
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

#include "visual_server_callbacks.h"

#include "core/object.h"

void VisualServerCallbacks::lock() {
	mutex.lock();
}

void VisualServerCallbacks::unlock() {
	mutex.unlock();
}

void VisualServerCallbacks::flush() {
	// should be ok without a lock ..
	// is the most common case and should be quicker
	if (!messages.size()) {
		return;
	}

	lock();
	for (int n = 0; n < messages.size(); n++) {
		const Message &mess = messages[n];

		Object *obj = ObjectDB::get_instance(mess.object_id);
		if (!obj) {
			continue;
		}

		obj->notification_callback(mess.type);
	}

	messages.clear();

	unlock();
}
