/**************************************************************************/
/*  rid.cpp                                                               */
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

#include "gdnative/rid.h"

#include "core/resource.h"
#include "core/rid.h"
#include "core/variant.h"

#ifdef __cplusplus
extern "C" {
#endif

static_assert(sizeof(godot_rid) == sizeof(RID), "RID size mismatch");

void GDAPI godot_rid_new(godot_rid *r_dest) {
	RID *dest = (RID *)r_dest;
	memnew_placement(dest, RID);
}

godot_int GDAPI godot_rid_get_id(const godot_rid *p_self) {
	const RID *self = (const RID *)p_self;
	return self->get_id();
}

void GDAPI godot_rid_new_with_resource(godot_rid *r_dest, const godot_object *p_from) {
	const Resource *res_from = Object::cast_to<Resource>((Object *)p_from);
	godot_rid_new(r_dest);
	if (res_from) {
		RID *dest = (RID *)r_dest;
		*dest = RID(res_from->get_rid());
	}
}

godot_bool GDAPI godot_rid_operator_equal(const godot_rid *p_self, const godot_rid *p_b) {
	const RID *self = (const RID *)p_self;
	const RID *b = (const RID *)p_b;
	return *self == *b;
}

godot_bool GDAPI godot_rid_operator_less(const godot_rid *p_self, const godot_rid *p_b) {
	const RID *self = (const RID *)p_self;
	const RID *b = (const RID *)p_b;
	return *self < *b;
}

#ifdef __cplusplus
}
#endif
