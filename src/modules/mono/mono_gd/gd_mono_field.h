/**************************************************************************/
/*  gd_mono_field.h                                                       */
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

#ifndef GD_MONO_FIELD_H
#define GD_MONO_FIELD_H

#include "gd_mono.h"
#include "gd_mono_header.h"
#include "i_mono_class_member.h"

class GDMonoField : public IMonoClassMember {
	GDMonoClass *owner;
	MonoClassField *mono_field;

	StringName name;
	ManagedType type;

	bool attrs_fetched;
	MonoCustomAttrInfo *attributes;

public:
	virtual GDMonoClass *get_enclosing_class() const GD_FINAL { return owner; }

	virtual MemberType get_member_type() const GD_FINAL { return MEMBER_TYPE_FIELD; }

	virtual StringName get_name() const GD_FINAL { return name; }

	virtual bool is_static() GD_FINAL;
	virtual Visibility get_visibility() GD_FINAL;

	virtual bool has_attribute(GDMonoClass *p_attr_class) GD_FINAL;
	virtual MonoObject *get_attribute(GDMonoClass *p_attr_class) GD_FINAL;
	void fetch_attributes();

	_FORCE_INLINE_ ManagedType get_type() const { return type; }

	void set_value_raw(MonoObject *p_object, void *p_ptr);
	void set_value_from_variant(MonoObject *p_object, const Variant &p_value);

	MonoObject *get_value(MonoObject *p_object);

	bool get_bool_value(MonoObject *p_object);
	int get_int_value(MonoObject *p_object);
	String get_string_value(MonoObject *p_object);

	GDMonoField(MonoClassField *p_mono_field, GDMonoClass *p_owner);
	~GDMonoField();
};

#endif // GD_MONO_FIELD_H
