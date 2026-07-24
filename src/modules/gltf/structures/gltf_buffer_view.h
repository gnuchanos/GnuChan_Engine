/**************************************************************************/
/*  gltf_buffer_view.h                                                    */
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

#ifndef GLTF_BUFFER_VIEW_H
#define GLTF_BUFFER_VIEW_H

#include "../gltf_defines.h"
#include "core/resource.h"

class GLTFBufferView : public Resource {
	GDCLASS(GLTFBufferView, Resource);
	friend class GLTFDocument;

private:
	GLTFBufferIndex buffer = -1;
	int byte_offset = 0;
	int byte_length = 0;
	int byte_stride = -1;
	bool indices = false;

protected:
	static void _bind_methods();

public:
	GLTFBufferIndex get_buffer();
	void set_buffer(GLTFBufferIndex p_buffer);

	int get_byte_offset();
	void set_byte_offset(int p_byte_offset);

	int get_byte_length();
	void set_byte_length(int p_byte_length);

	int get_byte_stride();
	void set_byte_stride(int p_byte_stride);

	bool get_indices();
	void set_indices(bool p_indices);
	// matrices need to be transformed to this
};

#endif // GLTF_BUFFER_VIEW_H
