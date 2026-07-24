/**************************************************************************/
/*  mono_gc_handle.h                                                      */
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

#ifndef MONO_GC_HANDLE_H
#define MONO_GC_HANDLE_H

#include <mono/jit/jit.h>

#include "core/reference.h"

namespace gdmono {

class MonoGCHandle : public Reference {
	GDCLASS(MonoGCHandle, Reference);

	bool released;
	bool weak;
	uint32_t handle;

public:
	enum HandleType {
		STRONG_HANDLE,
		WEAK_HANDLE
	};

	static uint32_t new_strong_handle(MonoObject *p_object);
	static uint32_t new_strong_handle_pinned(MonoObject *p_object);
	static uint32_t new_weak_handle(MonoObject *p_object);
	static void free_handle(uint32_t p_gchandle);

	static Ref<MonoGCHandle> create_strong(MonoObject *p_object);
	static Ref<MonoGCHandle> create_weak(MonoObject *p_object);

	_FORCE_INLINE_ bool is_released() { return released; }
	_FORCE_INLINE_ bool is_weak() { return weak; }

	_FORCE_INLINE_ MonoObject *get_target() const { return released ? NULL : mono_gchandle_get_target(handle); }

	_FORCE_INLINE_ void set_handle(uint32_t p_handle, HandleType p_handle_type) {
		released = false;
		weak = p_handle_type == WEAK_HANDLE;
		handle = p_handle;
	}
	void release();

	MonoGCHandle(uint32_t p_handle, HandleType p_handle_type);
	~MonoGCHandle();
};

} // namespace gdmono

#endif // MONO_GC_HANDLE_H
