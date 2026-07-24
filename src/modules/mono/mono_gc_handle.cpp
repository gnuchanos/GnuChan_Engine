/**************************************************************************/
/*  mono_gc_handle.cpp                                                    */
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

#include "mono_gc_handle.h"

#include "mono_gd/gd_mono.h"

namespace gdmono {

uint32_t MonoGCHandle::new_strong_handle(MonoObject *p_object) {
	return mono_gchandle_new(p_object, /* pinned: */ false);
}

uint32_t MonoGCHandle::new_strong_handle_pinned(MonoObject *p_object) {
	return mono_gchandle_new(p_object, /* pinned: */ true);
}

uint32_t MonoGCHandle::new_weak_handle(MonoObject *p_object) {
	return mono_gchandle_new_weakref(p_object, /* track_resurrection: */ false);
}

void MonoGCHandle::free_handle(uint32_t p_gchandle) {
	mono_gchandle_free(p_gchandle);
}

Ref<MonoGCHandle> MonoGCHandle::create_strong(MonoObject *p_object) {
	return memnew(MonoGCHandle(new_strong_handle(p_object), STRONG_HANDLE));
}

Ref<MonoGCHandle> MonoGCHandle::create_weak(MonoObject *p_object) {
	return memnew(MonoGCHandle(new_weak_handle(p_object), WEAK_HANDLE));
}

void MonoGCHandle::release() {
#ifdef DEBUG_ENABLED
	CRASH_COND(!released && GDMono::get_singleton() == NULL);
#endif

	if (!released && GDMono::get_singleton() && GDMono::get_singleton()->is_runtime_initialized()) {
		free_handle(handle);
		released = true;
	}
}

MonoGCHandle::MonoGCHandle(uint32_t p_handle, HandleType p_handle_type) {
	released = false;
	weak = p_handle_type == WEAK_HANDLE;
	handle = p_handle;
}

MonoGCHandle::~MonoGCHandle() {
	release();
}

} // namespace gdmono
