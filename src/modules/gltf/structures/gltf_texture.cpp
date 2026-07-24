/**************************************************************************/
/*  gltf_texture.cpp                                                      */
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

#include "gltf_texture.h"

void GLTFTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_src_image"), &GLTFTexture::get_src_image);
	ClassDB::bind_method(D_METHOD("set_src_image", "src_image"), &GLTFTexture::set_src_image);
	ClassDB::bind_method(D_METHOD("get_sampler"), &GLTFTexture::get_sampler);
	ClassDB::bind_method(D_METHOD("set_sampler", "sampler"), &GLTFTexture::set_sampler);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "src_image"), "set_src_image", "get_src_image"); // int
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sampler"), "set_sampler", "get_sampler"); // int
}

GLTFImageIndex GLTFTexture::get_src_image() const {
	return src_image;
}

void GLTFTexture::set_src_image(GLTFImageIndex val) {
	src_image = val;
}

GLTFTextureSamplerIndex GLTFTexture::get_sampler() const {
	return sampler;
}

void GLTFTexture::set_sampler(GLTFTextureSamplerIndex val) {
	sampler = val;
}
