/**************************************************************************/
/*  managed_type.cpp                                                      */
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

#include "managed_type.h"

#include "gd_mono.h"
#include "gd_mono_class.h"

ManagedType ManagedType::from_class(GDMonoClass *p_class) {
	return ManagedType(mono_type_get_type(p_class->get_mono_type()), p_class);
}

ManagedType ManagedType::from_class(MonoClass *p_mono_class) {
	GDMonoClass *tclass = GDMono::get_singleton()->get_class(p_mono_class);
	ERR_FAIL_COND_V(!tclass, ManagedType());

	return ManagedType(mono_type_get_type(tclass->get_mono_type()), tclass);
}

ManagedType ManagedType::from_type(MonoType *p_mono_type) {
	MonoClass *mono_class = mono_class_from_mono_type(p_mono_type);
	GDMonoClass *tclass = GDMono::get_singleton()->get_class(mono_class);
	ERR_FAIL_COND_V(!tclass, ManagedType());

	return ManagedType(mono_type_get_type(p_mono_type), tclass);
}

ManagedType ManagedType::from_reftype(MonoReflectionType *p_mono_reftype) {
	MonoType *mono_type = mono_reflection_type_get_type(p_mono_reftype);
	return from_type(mono_type);
}
