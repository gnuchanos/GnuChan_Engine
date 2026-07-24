/**************************************************************************/
/*  gltf_texture_sampler.cpp                                              */
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

#include "gltf_texture_sampler.h"

void GLTFTextureSampler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_mag_filter"), &GLTFTextureSampler::get_mag_filter);
	ClassDB::bind_method(D_METHOD("set_mag_filter", "filter_mode"), &GLTFTextureSampler::set_mag_filter);
	ClassDB::bind_method(D_METHOD("get_min_filter"), &GLTFTextureSampler::get_min_filter);
	ClassDB::bind_method(D_METHOD("set_min_filter", "filter_mode"), &GLTFTextureSampler::set_min_filter);
	ClassDB::bind_method(D_METHOD("get_wrap_s"), &GLTFTextureSampler::get_wrap_s);
	ClassDB::bind_method(D_METHOD("set_wrap_s", "wrap_mode"), &GLTFTextureSampler::set_wrap_s);
	ClassDB::bind_method(D_METHOD("get_wrap_t"), &GLTFTextureSampler::get_wrap_t);
	ClassDB::bind_method(D_METHOD("set_wrap_t", "wrap_mode"), &GLTFTextureSampler::set_wrap_t);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "mag_filter"), "set_mag_filter", "get_mag_filter");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "min_filter"), "set_min_filter", "get_min_filter");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "wrap_s"), "set_wrap_s", "get_wrap_s");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "wrap_t"), "set_wrap_t", "get_wrap_t");
}
