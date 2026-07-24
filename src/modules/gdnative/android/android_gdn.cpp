/**************************************************************************/
/*  android_gdn.cpp                                                       */
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

#include "modules/gdnative/gdnative.h"

// Code by Paritosh97 with minor tweaks by Mux213
// These entry points are only for the android platform and are simple stubs in all others.

#ifdef __ANDROID__
#include "platform/android/java_godot_wrapper.h"
#include "platform/android/os_android.h"
#include "platform/android/thread_jandroid.h"
#else
#define JNIEnv void
#define jobject void *
#endif

#ifdef __cplusplus
extern "C" {
#endif

JNIEnv *GDAPI godot_android_get_env() {
#ifdef __ANDROID__
	return get_jni_env();
#else
	return nullptr;
#endif
}

jobject GDAPI godot_android_get_activity() {
#ifdef __ANDROID__
	OS_Android *os_android = (OS_Android *)OS::get_singleton();
	return os_android->get_godot_java()->get_activity();
#else
	return nullptr;
#endif
}

jobject GDAPI godot_android_get_surface() {
#ifdef __ANDROID__
	OS_Android *os_android = (OS_Android *)OS::get_singleton();
	return os_android->get_godot_java()->get_surface();
#else
	return nullptr;
#endif
}

bool GDAPI godot_android_is_activity_resumed() {
#ifdef __ANDROID__
	OS_Android *os_android = (OS_Android *)OS::get_singleton();
	return os_android->get_godot_java()->is_activity_resumed();
#else
	return false;
#endif
}

#ifdef __cplusplus
}
#endif
