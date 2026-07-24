/**************************************************************************/
/*  version.h                                                             */
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

#ifndef VERSION_H
#define VERSION_H

#include "core/version_generated.gen.h"

// Copied from typedefs.h to stay lean.
#ifndef _STR
#define _STR(m_x) #m_x
#define _MKSTR(m_x) _STR(m_x)
#endif

// Version format: <major>.<minor> for the initial release,
// and <major>.<minor>.<patch> for subsequent releases where <patch> != 0.

// Defines the main "branch" version. Patch versions in this branch should be
// forward-compatible.
// Example: "0.1"
#define VERSION_BRANCH _MKSTR(VERSION_MAJOR) "." _MKSTR(VERSION_MINOR)
#if VERSION_PATCH
// Example: "0.1.4"
#define VERSION_NUMBER VERSION_BRANCH "." _MKSTR(VERSION_PATCH)
#else // patch is 0, we don't include it in the "pretty" version number.
// Example: "0.1" instead of "0.1.0"
#define VERSION_NUMBER VERSION_BRANCH
#endif // VERSION_PATCH

// Version number encoded as hexadecimal int with one byte for each number,
// for easy comparison from code.
// Example: 0.1.4 will be 0x000104, making comparison easy from script.
#define VERSION_HEX 0x10000 * VERSION_MAJOR + 0x100 * VERSION_MINOR + VERSION_PATCH

// Describes the full configuration of this engine version, including the
// version number, the status (gc, rc, stable, etc.) and potential
// module-specific features.
// Example: "0.1.0.gc"
#define VERSION_FULL_CONFIG VERSION_NUMBER "." VERSION_STATUS VERSION_MODULE_CONFIG

// Similar to VERSION_FULL_CONFIG, but also includes the (potentially custom)
// VERSION_BUILD description (e.g. official, custom_build, etc.).
// Example: "0.1.0.gc.official"
#define VERSION_FULL_BUILD VERSION_FULL_CONFIG "." VERSION_BUILD

// Same as above, but prepended with engine's name and a cosmetic "v".
// Example: "GnuChan Engine v0.1.0.gc.official"
#define VERSION_FULL_NAME VERSION_NAME " v" VERSION_FULL_BUILD

// Git commit hash, generated at build time in `core/version_hash.gen.cpp`.
extern const char *const VERSION_HASH;

#endif // VERSION_H
