/**************************************************************************/
/*  string_glue.cpp                                                       */
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

#include "string_glue.h"

#ifdef MONO_GLUE_ENABLED

#include "core/ustring.h"
#include "core/variant.h"
#include "core/vector.h"

MonoArray *godot_icall_String_md5_buffer(MonoString *p_str) {
	Vector<uint8_t> ret = GDMonoMarshal::mono_string_to_godot(p_str).md5_buffer();
	// TODO Check possible Array/Vector<uint8_t> problem?
	return GDMonoMarshal::Array_to_mono_array(Variant(ret));
}

MonoString *godot_icall_String_md5_text(MonoString *p_str) {
	String ret = GDMonoMarshal::mono_string_to_godot(p_str).md5_text();
	return GDMonoMarshal::mono_string_from_godot(ret);
}

int godot_icall_String_rfind(MonoString *p_str, MonoString *p_what, int p_from) {
	String what = GDMonoMarshal::mono_string_to_godot(p_what);
	return GDMonoMarshal::mono_string_to_godot(p_str).rfind(what, p_from);
}

int godot_icall_String_rfindn(MonoString *p_str, MonoString *p_what, int p_from) {
	String what = GDMonoMarshal::mono_string_to_godot(p_what);
	return GDMonoMarshal::mono_string_to_godot(p_str).rfindn(what, p_from);
}

MonoArray *godot_icall_String_sha256_buffer(MonoString *p_str) {
	Vector<uint8_t> ret = GDMonoMarshal::mono_string_to_godot(p_str).sha256_buffer();
	return GDMonoMarshal::Array_to_mono_array(Variant(ret));
}

MonoString *godot_icall_String_sha256_text(MonoString *p_str) {
	String ret = GDMonoMarshal::mono_string_to_godot(p_str).sha256_text();
	return GDMonoMarshal::mono_string_from_godot(ret);
}

MonoString *godot_icall_String_simplify_path(MonoString *p_str) {
	String ret = GDMonoMarshal::mono_string_to_godot(p_str).simplify_path();
	return GDMonoMarshal::mono_string_from_godot(ret);
}

void godot_register_string_icalls() {
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_md5_buffer", godot_icall_String_md5_buffer);
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_md5_text", godot_icall_String_md5_text);
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_rfind", godot_icall_String_rfind);
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_rfindn", godot_icall_String_rfindn);
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_sha256_buffer", godot_icall_String_sha256_buffer);
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_sha256_text", godot_icall_String_sha256_text);
	GDMonoUtils::add_internal_call("Godot.StringExtensions::godot_icall_String_simplify_path", godot_icall_String_simplify_path);
}

#endif // MONO_GLUE_ENABLED
