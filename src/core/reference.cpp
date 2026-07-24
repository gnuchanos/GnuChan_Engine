/**************************************************************************/
/*  reference.cpp                                                         */
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

#include "reference.h"

#include "core/script_language.h"

bool Reference::init_ref() {
	if (reference()) {
		if (!is_referenced() && refcount_init.unref()) {
			unreference(); // first referencing is already 1, so compensate for the ref above
		}

		return true;
	} else {
		return false;
	}
}

void Reference::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init_ref"), &Reference::init_ref);
	ClassDB::bind_method(D_METHOD("reference"), &Reference::reference);
	ClassDB::bind_method(D_METHOD("unreference"), &Reference::unreference);
}

int Reference::reference_get_count() const {
	return refcount.get();
}

bool Reference::reference() {
	uint32_t rc_val = refcount.refval();
	bool success = rc_val != 0;

	if (success && rc_val <= 2 /* higher is not relevant */) {
		if (get_script_instance()) {
			get_script_instance()->refcount_incremented();
		}
		if (instance_binding_count.get() > 0 && !ScriptServer::are_languages_finished()) {
			for (int i = 0; i < MAX_SCRIPT_INSTANCE_BINDINGS; i++) {
				if (_script_instance_bindings[i]) {
					ScriptServer::get_language(i)->refcount_incremented_instance_binding(this);
				}
			}
		}
	}

	return success;
}

bool Reference::unreference() {
	uint32_t rc_val = refcount.unrefval();
	bool die = rc_val == 0;

	if (rc_val <= 1 /* higher is not relevant */) {
		if (get_script_instance()) {
			bool script_ret = get_script_instance()->refcount_decremented();
			die = die && script_ret;
		}
		if (instance_binding_count.get() > 0 && !ScriptServer::are_languages_finished()) {
			for (int i = 0; i < MAX_SCRIPT_INSTANCE_BINDINGS; i++) {
				if (_script_instance_bindings[i]) {
					bool script_ret = ScriptServer::get_language(i)->refcount_decremented_instance_binding(this);
					die = die && script_ret;
				}
			}
		}
	}

	return die;
}

Reference::Reference() {
	refcount.init();
	refcount_init.init();
}

Reference::~Reference() {
}

Variant WeakRef::get_ref() const {
	if (ref == 0) {
		return Variant();
	}

	Object *obj = ObjectDB::get_instance(ref);
	if (!obj) {
		return Variant();
	}
	Reference *r = cast_to<Reference>(obj);
	if (r) {
		return REF(r);
	}

	return obj;
}

void WeakRef::set_obj(Object *p_object) {
	ref = p_object ? p_object->get_instance_id() : 0;
}

void WeakRef::set_ref(const REF &p_ref) {
	ref = p_ref.is_valid() ? p_ref->get_instance_id() : 0;
}

WeakRef::WeakRef() :
		ref(0) {
}

void WeakRef::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_ref"), &WeakRef::get_ref);
}
