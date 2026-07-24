/**************************************************************************/
/*  rid_glue.cpp                                                          */
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

#include "rid_glue.h"

#ifdef MONO_GLUE_ENABLED

#include "core/resource.h"

RID *godot_icall_RID_Ctor(Object *p_from) {
	Resource *res_from = Object::cast_to<Resource>(p_from);

	if (res_from)
		return memnew(RID(res_from->get_rid()));

	return memnew(RID);
}

void godot_icall_RID_Dtor(RID *p_ptr) {
	ERR_FAIL_NULL(p_ptr);
	memdelete(p_ptr);
}

uint32_t godot_icall_RID_get_id(RID *p_ptr) {
	return p_ptr->get_id();
}

void godot_register_rid_icalls() {
	GDMonoUtils::add_internal_call("Godot.RID::godot_icall_RID_Ctor", godot_icall_RID_Ctor);
	GDMonoUtils::add_internal_call("Godot.RID::godot_icall_RID_Dtor", godot_icall_RID_Dtor);
	GDMonoUtils::add_internal_call("Godot.RID::godot_icall_RID_get_id", godot_icall_RID_get_id);
}

#endif // MONO_GLUE_ENABLED
