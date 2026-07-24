/**************************************************************************/
/*  gltf_buffer_view.cpp                                                  */
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

#include "gltf_buffer_view.h"

void GLTFBufferView::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_buffer"), &GLTFBufferView::get_buffer);
	ClassDB::bind_method(D_METHOD("set_buffer", "buffer"), &GLTFBufferView::set_buffer);
	ClassDB::bind_method(D_METHOD("get_byte_offset"), &GLTFBufferView::get_byte_offset);
	ClassDB::bind_method(D_METHOD("set_byte_offset", "byte_offset"), &GLTFBufferView::set_byte_offset);
	ClassDB::bind_method(D_METHOD("get_byte_length"), &GLTFBufferView::get_byte_length);
	ClassDB::bind_method(D_METHOD("set_byte_length", "byte_length"), &GLTFBufferView::set_byte_length);
	ClassDB::bind_method(D_METHOD("get_byte_stride"), &GLTFBufferView::get_byte_stride);
	ClassDB::bind_method(D_METHOD("set_byte_stride", "byte_stride"), &GLTFBufferView::set_byte_stride);
	ClassDB::bind_method(D_METHOD("get_indices"), &GLTFBufferView::get_indices);
	ClassDB::bind_method(D_METHOD("set_indices", "indices"), &GLTFBufferView::set_indices);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "buffer"), "set_buffer", "get_buffer"); // GLTFBufferIndex
	ADD_PROPERTY(PropertyInfo(Variant::INT, "byte_offset"), "set_byte_offset", "get_byte_offset"); // int
	ADD_PROPERTY(PropertyInfo(Variant::INT, "byte_length"), "set_byte_length", "get_byte_length"); // int
	ADD_PROPERTY(PropertyInfo(Variant::INT, "byte_stride"), "set_byte_stride", "get_byte_stride"); // int
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "indices"), "set_indices", "get_indices"); // bool
}

GLTFBufferIndex GLTFBufferView::get_buffer() {
	return buffer;
}

void GLTFBufferView::set_buffer(GLTFBufferIndex p_buffer) {
	buffer = p_buffer;
}

int GLTFBufferView::get_byte_offset() {
	return byte_offset;
}

void GLTFBufferView::set_byte_offset(int p_byte_offset) {
	byte_offset = p_byte_offset;
}

int GLTFBufferView::get_byte_length() {
	return byte_length;
}

void GLTFBufferView::set_byte_length(int p_byte_length) {
	byte_length = p_byte_length;
}

int GLTFBufferView::get_byte_stride() {
	return byte_stride;
}

void GLTFBufferView::set_byte_stride(int p_byte_stride) {
	byte_stride = p_byte_stride;
}

bool GLTFBufferView::get_indices() {
	return indices;
}

void GLTFBufferView::set_indices(bool p_indices) {
	indices = p_indices;
}
