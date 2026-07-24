/**************************************************************************/
/*  osx_utils.cpp                                                         */
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

#include "osx_utils.h"

#ifdef OSX_ENABLED

#include "core/print_string.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

bool osx_is_app_bundle_installed(const String &p_bundle_id) {
	CFStringRef bundle_id = CFStringCreateWithCString(nullptr, p_bundle_id.utf8(), kCFStringEncodingUTF8);
	CFArrayRef result = LSCopyApplicationURLsForBundleIdentifier(bundle_id, nullptr);
	CFRelease(bundle_id);

	if (result) {
		if (CFArrayGetCount(result) > 0) {
			CFRelease(result);
			return true;
		} else {
			CFRelease(result);
			return false;
		}
	} else {
		return false;
	}
}

#endif
