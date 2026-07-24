/**************************************************************************/
/*  denoise_wrapper.cpp                                                   */
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

#include "denoise_wrapper.h"
#include "core/os/memory.h"
#include "thirdparty/oidn/include/OpenImageDenoise/oidn.h"
#include <stdio.h>

void *oidn_denoiser_init() {
	OIDNDeviceImpl *device = oidnNewDevice(OIDN_DEVICE_TYPE_CPU);
	oidnCommitDevice(device);
	return device;
}

bool oidn_denoise(void *deviceptr, float *p_floats, int p_width, int p_height) {
	OIDNDeviceImpl *device = (OIDNDeviceImpl *)deviceptr;
	OIDNFilter filter = oidnNewFilter(device, "RTLightmap");
	oidnSetSharedFilterImage(filter, "color", (void *)p_floats, OIDN_FORMAT_FLOAT3, p_width, p_height, 0, 0, 0);
	oidnSetSharedFilterImage(filter, "output", (void *)p_floats, OIDN_FORMAT_FLOAT3, p_width, p_height, 0, 0, 0);
	oidnSetFilter1b(filter, "hdr", true);
	oidnCommitFilter(filter);
	oidnExecuteFilter(filter);

	const char *msg;
	bool success = true;
	if (oidnGetDeviceError(device, &msg) != OIDN_ERROR_NONE) {
		printf("LightmapDenoiser: %s\n", msg);
		success = false;
	}

	oidnReleaseFilter(filter);
	return success;
}

void oidn_denoiser_finish(void *device) {
	oidnReleaseDevice((OIDNDeviceImpl *)device);
}
