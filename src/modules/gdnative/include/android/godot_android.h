/**************************************************************************/
/*  godot_android.h                                                       */
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

#ifndef GODOT_ANDROID_H
#define GODOT_ANDROID_H

#include <gdnative/gdnative.h>

#ifdef __ANDROID__
#include <jni.h>
#else
#define JNIEnv void
#define jobject void *
#endif

#ifdef __cplusplus
extern "C" {
#endif

JNIEnv *GDAPI godot_android_get_env();
jobject GDAPI godot_android_get_activity();
jobject GDAPI godot_android_get_surface();
bool GDAPI godot_android_is_activity_resumed();

#ifdef __cplusplus
}
#endif

#endif // GODOT_ANDROID_H
