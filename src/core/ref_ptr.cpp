/**************************************************************************/
/*  ref_ptr.cpp                                                           */
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

#include "ref_ptr.h"

#include "core/reference.h"
#include "core/resource.h"

void RefPtr::operator=(const RefPtr &p_other) {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	Ref<Reference> *ref_other = reinterpret_cast<Ref<Reference> *>(const_cast<char *>(&p_other.data[0]));

	*ref = *ref_other;
}

bool RefPtr::operator==(const RefPtr &p_other) const {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	Ref<Reference> *ref_other = reinterpret_cast<Ref<Reference> *>(const_cast<char *>(&p_other.data[0]));

	return *ref == *ref_other;
}

bool RefPtr::operator!=(const RefPtr &p_other) const {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	Ref<Reference> *ref_other = reinterpret_cast<Ref<Reference> *>(const_cast<char *>(&p_other.data[0]));

	return *ref != *ref_other;
}

RefPtr::RefPtr(const RefPtr &p_other) {
	memnew_placement(&data[0], Ref<Reference>);

	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	Ref<Reference> *ref_other = reinterpret_cast<Ref<Reference> *>(const_cast<char *>(&p_other.data[0]));

	*ref = *ref_other;
}

bool RefPtr::is_null() const {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	return ref->is_null();
}

RID RefPtr::get_rid() const {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	if (ref->is_null()) {
		return RID();
	}
	Resource *res = Object::cast_to<Resource>(ref->ptr());
	if (res) {
		return res->get_rid();
	}
	return RID();
}

void RefPtr::unref() {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	ref->unref();
}

RefPtr::RefPtr() {
	ERR_FAIL_COND(sizeof(Ref<Reference>) > DATASIZE);
	memnew_placement(&data[0], Ref<Reference>);
}

RefPtr::~RefPtr() {
	Ref<Reference> *ref = reinterpret_cast<Ref<Reference> *>(&data[0]);
	ref->~Ref<Reference>();
}
