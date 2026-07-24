/**************************************************************************/
/*  shader_cache_gles4.h                                                  */
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

#ifndef SHADER_CACHE_GLES4_H
#define SHADER_CACHE_GLES4_H

#include "core/local_vector.h"
#include "core/reference.h"

class DirAccess;
class String;

class ShaderCacheGLES4 {
	DirAccess *storage_da;
	String storage_path;
	uint64_t storage_size = 0;

	void _purge_excess();

public:
	static ShaderCacheGLES4 *singleton;

	static String hash_program(const char *const *p_platform_strings, const LocalVector<const char *> &p_vertex_strings, const LocalVector<const char *> &p_fragment_strings);

	bool retrieve(const String &p_program_hash, uint32_t *r_format, PoolByteArray *r_data);
	void store(const String &p_program_hash, uint32_t p_program_format, const PoolByteArray &p_program_data);
	void remove(const String &p_program_hash);

	ShaderCacheGLES4();
	~ShaderCacheGLES4();
};

#endif // SHADER_CACHE_GLES4_H
