/**************************************************************************/
/*  visual_server_callbacks.h                                             */
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

#ifndef VISUAL_SERVER_CALLBACKS_H
#define VISUAL_SERVER_CALLBACKS_H

#include "core/local_vector.h"
#include "core/object_id.h"
#include "core/os/mutex.h"

class VisualServerCallbacks {
public:
	enum CallbackType {
		CALLBACK_NOTIFICATION_ENTER_GAMEPLAY,
		CALLBACK_NOTIFICATION_EXIT_GAMEPLAY,
		CALLBACK_SIGNAL_ENTER_GAMEPLAY,
		CALLBACK_SIGNAL_EXIT_GAMEPLAY,
	};

	struct Message {
		CallbackType type;
		ObjectID object_id;
	};

	void lock();
	void unlock();
	void flush();

	void push_message(const Message &p_message) { messages.push_back(p_message); }
	int32_t get_num_messages() const { return messages.size(); }
	const Message &get_message(int p_index) const { return messages[p_index]; }
	void clear() { messages.clear(); }

private:
	LocalVector<Message, int32_t> messages;
	Mutex mutex;
};

#endif // VISUAL_SERVER_CALLBACKS_H
