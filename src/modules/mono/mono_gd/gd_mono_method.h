/**************************************************************************/
/*  gd_mono_method.h                                                      */
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

#ifndef GD_MONO_METHOD_H
#define GD_MONO_METHOD_H

#include "gd_mono.h"
#include "gd_mono_header.h"
#include "i_mono_class_member.h"

class GDMonoMethod : public IMonoClassMember {
	StringName name;

	uint16_t params_count;
	unsigned int params_buffer_size;
	ManagedType return_type;
	Vector<ManagedType> param_types;

	bool method_info_fetched;
	MethodInfo method_info;

	bool attrs_fetched;
	MonoCustomAttrInfo *attributes;

	void _update_signature();
	void _update_signature(MonoMethodSignature *p_method_sig);

	friend class GDMonoClass;

	MonoMethod *mono_method;

public:
	virtual GDMonoClass *get_enclosing_class() const GD_FINAL;

	virtual MemberType get_member_type() const GD_FINAL { return MEMBER_TYPE_METHOD; }

	virtual StringName get_name() const GD_FINAL { return name; }

	virtual bool is_static() GD_FINAL;

	virtual Visibility get_visibility() GD_FINAL;

	virtual bool has_attribute(GDMonoClass *p_attr_class) GD_FINAL;
	virtual MonoObject *get_attribute(GDMonoClass *p_attr_class) GD_FINAL;
	void fetch_attributes();

	_FORCE_INLINE_ MonoMethod *get_mono_ptr() const { return mono_method; }

	_FORCE_INLINE_ uint16_t get_parameters_count() const { return params_count; }
	_FORCE_INLINE_ ManagedType get_return_type() const { return return_type; }

	MonoObject *invoke(MonoObject *p_object, const Variant **p_params, MonoException **r_exc = NULL) const;
	MonoObject *invoke(MonoObject *p_object, MonoException **r_exc = NULL) const;
	MonoObject *invoke_raw(MonoObject *p_object, void **p_params, MonoException **r_exc = NULL) const;

	String get_full_name(bool p_signature = false) const;
	String get_full_name_no_class() const;
	String get_ret_type_full_name() const;
	String get_signature_desc(bool p_namespaces = false) const;

	void get_parameter_names(Vector<StringName> &names) const;
	void get_parameter_types(Vector<ManagedType> &types) const;

	const MethodInfo &get_method_info();

	GDMonoMethod(StringName p_name, MonoMethod *p_method);
	~GDMonoMethod();
};

#endif // GD_MONO_METHOD_H
