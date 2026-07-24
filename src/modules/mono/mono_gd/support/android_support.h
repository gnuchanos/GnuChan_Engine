/**************************************************************************/
/*  android_support.h                                                     */
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

#ifndef ANDROID_SUPPORT_H
#define ANDROID_SUPPORT_H

#if defined(ANDROID_ENABLED)

#include "core/ustring.h"

namespace gdmono {
namespace android {
namespace support {

String get_app_native_lib_dir();

void initialize();
void cleanup();

void register_internal_calls();

} // namespace support
} // namespace android
} // namespace gdmono

#endif // ANDROID_ENABLED

#endif // ANDROID_SUPPORT_H
