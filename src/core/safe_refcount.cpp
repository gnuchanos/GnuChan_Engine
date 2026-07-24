/**************************************************************************/
/*  safe_refcount.cpp                                                     */
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

#if defined(DEBUG_ENABLED) && !defined(NO_THREADS)

#include "safe_refcount.h"

#include "core/error_macros.h"

// On C++14 we don't have std::atomic::is_always_lockfree, so this is the best we can do
void check_lockless_atomics() {
	// Doing the check for the types we actually care about
	if (!std::atomic<uint32_t>{}.is_lock_free() || !std::atomic<uint64_t>{}.is_lock_free() || !std::atomic_bool{}.is_lock_free()) {
		WARN_PRINT("Your compiler doesn't seem to support lockless atomics. Performance will be degraded. Please consider upgrading to a different or newer compiler.");
	}
}

#endif
