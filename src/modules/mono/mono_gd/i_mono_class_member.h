/**************************************************************************/
/*  i_mono_class_member.h                                                 */
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

#ifndef I_MONO_CLASS_MEMBER_H
#define I_MONO_CLASS_MEMBER_H

#include "gd_mono_header.h"

#include <mono/metadata/object.h>

class IMonoClassMember {
public:
	enum Visibility {
		PRIVATE,
		PROTECTED_AND_INTERNAL, // FAM_AND_ASSEM
		INTERNAL, // ASSEMBLY
		PROTECTED, // FAMILY
		PUBLIC
	};

	enum MemberType {
		MEMBER_TYPE_FIELD,
		MEMBER_TYPE_PROPERTY,
		MEMBER_TYPE_METHOD
	};

	virtual ~IMonoClassMember() {}

	virtual GDMonoClass *get_enclosing_class() const = 0;

	virtual MemberType get_member_type() const = 0;

	virtual StringName get_name() const = 0;

	virtual bool is_static() = 0;

	virtual Visibility get_visibility() = 0;

	virtual bool has_attribute(GDMonoClass *p_attr_class) = 0;
	virtual MonoObject *get_attribute(GDMonoClass *p_attr_class) = 0;
};

#endif // I_MONO_CLASS_MEMBER_H
