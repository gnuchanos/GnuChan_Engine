/**************************************************************************/
/*  rasterizer_asserts.h                                                  */
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

#ifndef RASTERIZER_ASSERTS_H
#define RASTERIZER_ASSERTS_H

// For flow control checking, we want an easy way to apply asserts that occur in debug development builds only.
// This is enforced by outputting a warning which will fail CI checks if the define is set in a PR.
#if defined(TOOLS_ENABLED) && defined(DEBUG_ENABLED)
// only uncomment this define for error checking in development, not in the main repository
// as these checks will slow things down in debug builds.
//#define RASTERIZER_EXTRA_CHECKS
#endif

#ifdef RASTERIZER_EXTRA_CHECKS
#ifndef _MSC_VER
#warning do not define RASTERIZER_EXTRA_CHECKS in main repository builds
#endif
#define RAST_DEV_DEBUG_ASSERT(a) CRASH_COND(!(a))
#else
#define RAST_DEV_DEBUG_ASSERT(a)
#endif

// Also very useful, an assert check that only occurs in debug tools builds
#if defined(TOOLS_ENABLED) && defined(DEBUG_ENABLED)
#define RAST_DEBUG_ASSERT(a) CRASH_COND(!(a))
#else
#define RAST_DEBUG_ASSERT(a)
#endif

// Thin wrapper around ERR_FAIL_COND to allow us to make it debug only
#ifdef DEBUG_ENABLED
#define RAST_FAIL_COND(m_cond) ERR_FAIL_COND(m_cond)
#else
#define RAST_FAIL_COND(m_cond) \
	if (m_cond) {              \
	}
#endif

#endif // RASTERIZER_ASSERTS_H
