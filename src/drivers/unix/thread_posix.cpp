/**************************************************************************/
/*  thread_posix.cpp                                                      */
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

#if (defined(UNIX_ENABLED) || defined(PTHREAD_ENABLED)) && !defined(NO_THREADS)

#include "thread_posix.h"

#include "core/os/thread.h"
#include "core/ustring.h"

#ifdef PTHREAD_BSD_SET_NAME
#include <pthread_np.h>
#endif

static Error set_name(const String &p_name) {
#ifdef PTHREAD_NO_RENAME
	return ERR_UNAVAILABLE;

#else

#ifdef PTHREAD_RENAME_SELF

	// check if thread is the same as caller
	int err = pthread_setname_np(p_name.utf8().get_data());

#else

	pthread_t running_thread = pthread_self();
#ifdef PTHREAD_BSD_SET_NAME
	pthread_set_name_np(running_thread, p_name.utf8().get_data());
	int err = 0; // Open/FreeBSD ignore errors in this function
#elif defined(PTHREAD_NETBSD_SET_NAME)
	int err = pthread_setname_np(running_thread, "%s", const_cast<char *>(p_name.utf8().get_data()));
#else
	int err = pthread_setname_np(running_thread, p_name.utf8().get_data());
#endif // PTHREAD_BSD_SET_NAME

#endif // PTHREAD_RENAME_SELF

	return err == 0 ? OK : ERR_INVALID_PARAMETER;

#endif // PTHREAD_NO_RENAME
}

void init_thread_posix() {
	Thread::_set_platform_funcs(&set_name, nullptr);
}

#endif
