/**************************************************************************/
/*  gltf_accessor.h                                                       */
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

#ifndef GLTF_ACCESSOR_H
#define GLTF_ACCESSOR_H

#include "../gltf_defines.h"
#include "core/resource.h"

struct GLTFAccessor : public Resource {
	GDCLASS(GLTFAccessor, Resource);
	friend class GLTFDocument;

private:
	GLTFBufferViewIndex buffer_view = 0;
	int byte_offset = 0;
	int component_type = 0;
	bool normalized = false;
	int count = 0;
	GLTFType type = GLTFType::TYPE_SCALAR;
	PoolVector<float> min;
	PoolVector<float> max;
	int sparse_count = 0;
	int sparse_indices_buffer_view = 0;
	int sparse_indices_byte_offset = 0;
	int sparse_indices_component_type = 0;
	int sparse_values_buffer_view = 0;
	int sparse_values_byte_offset = 0;

protected:
	static void _bind_methods();

public:
	GLTFBufferViewIndex get_buffer_view();
	void set_buffer_view(GLTFBufferViewIndex p_buffer_view);

	int get_byte_offset();
	void set_byte_offset(int p_byte_offset);

	int get_component_type();
	void set_component_type(int p_component_type);

	bool get_normalized();
	void set_normalized(bool p_normalized);

	int get_count();
	void set_count(int p_count);

	int get_type();
	void set_type(int p_type);

	PoolVector<float> get_min();
	void set_min(PoolVector<float> p_min);

	PoolVector<float> get_max();
	void set_max(PoolVector<float> p_max);

	int get_sparse_count();
	void set_sparse_count(int p_sparse_count);

	int get_sparse_indices_buffer_view();
	void set_sparse_indices_buffer_view(int p_sparse_indices_buffer_view);

	int get_sparse_indices_byte_offset();
	void set_sparse_indices_byte_offset(int p_sparse_indices_byte_offset);

	int get_sparse_indices_component_type();
	void set_sparse_indices_component_type(int p_sparse_indices_component_type);

	int get_sparse_values_buffer_view();
	void set_sparse_values_buffer_view(int p_sparse_values_buffer_view);

	int get_sparse_values_byte_offset();
	void set_sparse_values_byte_offset(int p_sparse_values_byte_offset);
};

#endif // GLTF_ACCESSOR_H
