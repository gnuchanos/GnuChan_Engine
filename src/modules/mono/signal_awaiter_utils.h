/**************************************************************************/
/*  signal_awaiter_utils.h                                                */
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

#ifndef SIGNAL_AWAITER_UTILS_H
#define SIGNAL_AWAITER_UTILS_H

#include "core/reference.h"
#include "mono_gc_handle.h"

namespace SignalAwaiterUtils {

Error connect_signal_awaiter(Object *p_source, const String &p_signal, Object *p_target, MonoObject *p_awaiter);
}

class SignalAwaiterHandle : public gdmono::MonoGCHandle {
	GDCLASS(SignalAwaiterHandle, gdmono::MonoGCHandle);

	bool completed;

#ifdef DEBUG_ENABLED
	ObjectID conn_target_id;
#endif

	Variant _signal_callback(const Variant **p_args, int p_argcount, Variant::CallError &r_error);

protected:
	static void _bind_methods();

public:
	_FORCE_INLINE_ bool is_completed() { return completed; }
	_FORCE_INLINE_ void set_completed(bool p_completed) { completed = p_completed; }

#ifdef DEBUG_ENABLED
	_FORCE_INLINE_ void set_connection_target(Object *p_target) {
		conn_target_id = p_target->get_instance_id();
	}
#endif

	SignalAwaiterHandle(MonoObject *p_managed);
	~SignalAwaiterHandle();
};

#endif // SIGNAL_AWAITER_UTILS_H
