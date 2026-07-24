/**************************************************************************/
/*  string_name.cpp                                                       */
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

#include "gdnative/string_name.h"

#include "core/string_name.h"
#include "core/ustring.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static_assert(sizeof(godot_string_name) == sizeof(StringName), "StringName size mismatch");

void GDAPI godot_string_name_new(godot_string_name *r_dest, const godot_string *p_name) {
	StringName *dest = (StringName *)r_dest;
	const String *name = (const String *)p_name;
	memnew_placement(dest, StringName(*name));
}

void GDAPI godot_string_name_new_data(godot_string_name *r_dest, const char *p_name) {
	StringName *dest = (StringName *)r_dest;
	memnew_placement(dest, StringName(p_name));
}

godot_string GDAPI godot_string_name_get_name(const godot_string_name *p_self) {
	godot_string ret;
	const StringName *self = (const StringName *)p_self;
	memnew_placement(&ret, String(*self));
	return ret;
}

uint32_t GDAPI godot_string_name_get_hash(const godot_string_name *p_self) {
	const StringName *self = (const StringName *)p_self;
	return self->hash();
}

const void GDAPI *godot_string_name_get_data_unique_pointer(const godot_string_name *p_self) {
	const StringName *self = (const StringName *)p_self;
	return self->data_unique_pointer();
}

godot_bool GDAPI godot_string_name_operator_equal(const godot_string_name *p_self, const godot_string_name *p_other) {
	const StringName *self = (const StringName *)p_self;
	const StringName *other = (const StringName *)p_other;
	return *self == *other;
}

godot_bool GDAPI godot_string_name_operator_less(const godot_string_name *p_self, const godot_string_name *p_other) {
	const StringName *self = (const StringName *)p_self;
	const StringName *other = (const StringName *)p_other;
	return *self < *other;
}

void GDAPI godot_string_name_destroy(godot_string_name *p_self) {
	StringName *self = (StringName *)p_self;
	self->~StringName();
}

#ifdef __cplusplus
}
#endif
