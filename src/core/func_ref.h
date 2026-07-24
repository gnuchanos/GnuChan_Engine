/**************************************************************************/
/*  func_ref.h                                                            */
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

#ifndef FUNC_REF_H
#define FUNC_REF_H

#include "core/reference.h"

class FuncRef : public Reference {
	GDCLASS(FuncRef, Reference);
	ObjectID id;
	StringName function;

protected:
	static void _bind_methods();

public:
	Variant call_func(const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	Variant call_funcv(const Array &p_args);
	void set_instance(Object *p_obj);
	void set_function(const StringName &p_func);
	StringName get_function();
	bool is_valid() const;
	FuncRef();
};

#endif // FUNC_REF_H
