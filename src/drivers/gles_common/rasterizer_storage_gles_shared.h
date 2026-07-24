/**************************************************************************/
/*  rasterizer_storage_gles_shared.h                                      */
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

#ifndef RASTERIZER_STORAGE_GLES_SHARED_H
#define RASTERIZER_STORAGE_GLES_SHARED_H

// Shared helper for GLES3 and GLES4 rasterizers.
// This header is included by both rasterizer_storage_gles3.h and
// rasterizer_storage_gles4.h to share common GL utilities without
// violating the ODR when both are included in the same translation unit
// (e.g. os_windows.cpp includes both GLES3 and GLES4 driver headers).

#include "platform_config.h"
#ifndef GLES3_INCLUDE_H
#include <GLES3/gl3.h>
#else
#include GLES3_INCLUDE_H
#endif

#ifndef GL_TEXSTORAGE2DCUSTOM_DEFINED
#define GL_TEXSTORAGE2DCUSTOM_DEFINED

static inline void glTexStorage2DCustom(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type) {
#ifdef GLES_OVER_GL

	for (int i = 0; i < levels; i++) {
		glTexImage2D(target, i, internalformat, width, height, 0, format, type, nullptr);
		width = MAX(1, (width / 2));
		height = MAX(1, (height / 2));
	}

#else
	glTexStorage2D(target, levels, internalformat, width, height);
#endif
}

#endif // GL_TEXSTORAGE2DCUSTOM_DEFINED

#endif // RASTERIZER_STORAGE_GLES_SHARED_H
