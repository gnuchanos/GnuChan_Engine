/**************************************************************************/
/*  gd_mono_wasm_m2n.cpp                                                  */
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

#include "gd_mono_wasm_m2n.h"

#ifdef JAVASCRIPT_ENABLED

#include "core/oa_hash_map.h"

typedef mono_bool (*GodotMonoM2nIcallTrampolineDispatch)(const char *cookie, void *target_func, Mono_InterpMethodArguments *margs);

// This extern function is implemented in our patched version of Mono
MONO_API void godot_mono_register_m2n_icall_trampoline_dispatch_hook(GodotMonoM2nIcallTrampolineDispatch hook);

namespace GDMonoWasmM2n {

struct HashMapCookieComparator {
	static bool compare(const char *p_lhs, const char *p_rhs) {
		return strcmp(p_lhs, p_rhs) == 0;
	}
};

// The default hasher supports 'const char *' C Strings, but we need a custom comparator
OAHashMap<const char *, TrampolineFunc, HashMapHasherDefault, HashMapCookieComparator> trampolines;

void set_trampoline(const char *cookies, GDMonoWasmM2n::TrampolineFunc trampoline_func) {
	trampolines.set(cookies, trampoline_func);
}

mono_bool trampoline_dispatch_hook(const char *cookie, void *target_func, Mono_InterpMethodArguments *margs) {
	TrampolineFunc trampoline_func;

	if (!trampolines.lookup(cookie, trampoline_func)) {
		return false;
	}

	(*trampoline_func)(target_func, margs);
	return true;
}

bool initialized = false;

void lazy_initialize() {
	// Doesn't need to be thread safe
	if (!initialized) {
		initialized = true;
		godot_mono_register_m2n_icall_trampoline_dispatch_hook(&trampoline_dispatch_hook);
	}
}
} // namespace GDMonoWasmM2n

#endif
