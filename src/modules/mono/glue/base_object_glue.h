/**************************************************************************/
/*  base_object_glue.h                                                    */
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

#ifndef BASE_OBJECT_GLUE_H
#define BASE_OBJECT_GLUE_H

#ifdef MONO_GLUE_ENABLED

#include "core/class_db.h"
#include "core/object.h"

#include "../mono_gd/gd_mono_marshal.h"

Object *godot_icall_Object_Ctor(MonoObject *p_obj);

void godot_icall_Object_Disposed(MonoObject *p_obj, Object *p_ptr);

void godot_icall_Reference_Disposed(MonoObject *p_obj, Object *p_ptr, MonoBoolean p_is_finalizer);

MethodBind *godot_icall_Object_ClassDB_get_method(MonoString *p_type, MonoString *p_method);

MonoObject *godot_icall_Object_weakref(Object *p_obj);

int32_t godot_icall_SignalAwaiter_connect(Object *p_source, MonoString *p_signal, Object *p_target, MonoObject *p_awaiter);

// DynamicGodotObject

MonoArray *godot_icall_DynamicGodotObject_SetMemberList(Object *p_ptr);

MonoBoolean godot_icall_DynamicGodotObject_InvokeMember(Object *p_ptr, MonoString *p_name, MonoArray *p_args, MonoObject **r_result);

MonoBoolean godot_icall_DynamicGodotObject_GetMember(Object *p_ptr, MonoString *p_name, MonoObject **r_result);

MonoBoolean godot_icall_DynamicGodotObject_SetMember(Object *p_ptr, MonoString *p_name, MonoObject *p_value);

MonoString *godot_icall_Object_ToString(Object *p_ptr);

// Register internal calls

void godot_register_object_icalls();

#endif // MONO_GLUE_ENABLED

#endif // BASE_OBJECT_GLUE_H
