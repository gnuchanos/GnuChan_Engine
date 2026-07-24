/**************************************************************************/
/*  cull_instance.cpp                                                     */
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

#include "cull_instance.h"

VARIANT_ENUM_CAST(CullInstance::PortalMode);

void CullInstance::set_portal_mode(CullInstance::PortalMode p_mode) {
	if (p_mode != _portal_mode) {
		_portal_mode = p_mode;
		_refresh_portal_mode();
	}
}

CullInstance::PortalMode CullInstance::get_portal_mode() const {
	return _portal_mode;
}

void CullInstance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_portal_mode", "mode"), &CullInstance::set_portal_mode);
	ClassDB::bind_method(D_METHOD("get_portal_mode"), &CullInstance::get_portal_mode);

	ClassDB::bind_method(D_METHOD("set_include_in_bound", "enabled"), &CullInstance::set_include_in_bound);
	ClassDB::bind_method(D_METHOD("get_include_in_bound"), &CullInstance::get_include_in_bound);

	ClassDB::bind_method(D_METHOD("set_allow_merging", "enabled"), &CullInstance::set_allow_merging);
	ClassDB::bind_method(D_METHOD("get_allow_merging"), &CullInstance::get_allow_merging);

	ClassDB::bind_method(D_METHOD("set_portal_autoplace_priority", "priority"), &CullInstance::set_portal_autoplace_priority);
	ClassDB::bind_method(D_METHOD("get_portal_autoplace_priority"), &CullInstance::get_portal_autoplace_priority);

	ADD_GROUP("Portals", "");

	BIND_ENUM_CONSTANT(PORTAL_MODE_STATIC);
	BIND_ENUM_CONSTANT(PORTAL_MODE_DYNAMIC);
	BIND_ENUM_CONSTANT(PORTAL_MODE_ROAMING);
	BIND_ENUM_CONSTANT(PORTAL_MODE_GLOBAL);
	BIND_ENUM_CONSTANT(PORTAL_MODE_IGNORE);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "portal_mode", PROPERTY_HINT_ENUM, "Static,Dynamic,Roaming,Global,Ignore"), "set_portal_mode", "get_portal_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "include_in_bound"), "set_include_in_bound", "get_include_in_bound");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_merging"), "set_allow_merging", "get_allow_merging");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "autoplace_priority", PROPERTY_HINT_RANGE, "-16,16,1", PROPERTY_USAGE_DEFAULT), "set_portal_autoplace_priority", "get_portal_autoplace_priority");
}

CullInstance::CullInstance() {
	_portal_mode = PORTAL_MODE_STATIC;
	_include_in_bound = true;
	_allow_merging = true;
	_portal_autoplace_priority = 0;
}
