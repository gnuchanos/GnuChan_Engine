/**************************************************************************/
/*  gd_mono_internals.h                                                   */
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

#ifndef GD_MONO_INTERNALS_H
#define GD_MONO_INTERNALS_H

#include <mono/jit/jit.h>

#include "../utils/macros.h"

#include "core/object.h"

namespace GDMonoInternals {
void tie_managed_to_unmanaged(MonoObject *managed, Object *unmanaged);

/**
 * Do not call this function directly.
 * Use GDMonoUtils::debug_unhandled_exception(MonoException *) instead.
 */
void unhandled_exception(MonoException *p_exc);

void gd_unhandled_exception_event(MonoException *p_exc);

void check_call_error(const String &p_method, const Variant *p_instance, const Variant **p_args, int p_arg_count, const Variant::CallError &p_error);
} // namespace GDMonoInternals

#endif // GD_MONO_INTERNALS_H
