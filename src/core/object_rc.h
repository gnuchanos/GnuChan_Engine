/**************************************************************************/
/*  object_rc.h                                                           */
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

#ifndef OBJECT_RC_H
#define OBJECT_RC_H

#include "core/os/memory.h"
#include "core/typedefs.h"

#include <atomic>

class Object;

// Used to track Variants pointing to a non-Reference Object
class ObjectRC {
	std::atomic<Object *> _ptr;
	std::atomic<uint32_t> _users;

public:
	// This is for allowing debug builds to check for instance ID validity,
	// so warnings are shown in debug builds when a stray Variant (one pointing
	// to a released Object) would have happened.
	const ObjectID instance_id;

	_FORCE_INLINE_ void increment() {
		_users.fetch_add(1, std::memory_order_relaxed);
	}

	_FORCE_INLINE_ bool decrement() {
		return _users.fetch_sub(1, std::memory_order_relaxed) == 1;
	}

	_FORCE_INLINE_ bool invalidate() {
		_ptr.store(nullptr, std::memory_order_release);
		return decrement();
	}

	_FORCE_INLINE_ Object *get_ptr() {
		return _ptr.load(std::memory_order_acquire);
	}

	_FORCE_INLINE_ ObjectRC(Object *p_object) :
			instance_id(p_object->get_instance_id()) {
		// 1 (the Object) + 1 (the first user)
		_users.store(2, std::memory_order_relaxed);
		_ptr.store(p_object, std::memory_order_release);
	}
};

#endif // OBJECT_RC_H
