/**************************************************************************/
/*  spin_lock.h                                                           */
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

#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include "core/typedefs.h"

#include <atomic>

class SpinLock {
	std::atomic_flag locked = ATOMIC_FLAG_INIT;

public:
	_ALWAYS_INLINE_ void lock() {
		while (locked.test_and_set(std::memory_order_acquire)) {
			;
		}
	}
	_ALWAYS_INLINE_ void unlock() {
		locked.clear(std::memory_order_release);
	}
};

#endif // SPIN_LOCK_H
