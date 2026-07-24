/**************************************************************************/
/*  managed_type.h                                                        */
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

#ifndef MANAGED_TYPE_H
#define MANAGED_TYPE_H

#include <mono/metadata/object.h>

#include "gd_mono_header.h"

struct ManagedType {
	int type_encoding;
	GDMonoClass *type_class;

	static ManagedType from_class(GDMonoClass *p_class);
	static ManagedType from_class(MonoClass *p_mono_class);
	static ManagedType from_type(MonoType *p_mono_type);
	static ManagedType from_reftype(MonoReflectionType *p_mono_reftype);

	ManagedType() :
			type_encoding(0),
			type_class(NULL) {
	}

	ManagedType(int p_type_encoding, GDMonoClass *p_type_class) :
			type_encoding(p_type_encoding),
			type_class(p_type_class) {
	}
};

#endif // MANAGED_TYPE_H
