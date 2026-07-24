/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "core/project_settings.h"
#include "lightmapper_cpu.h"
#include "scene/3d/lightmapper.h"

#ifndef _3D_DISABLED
static Lightmapper *create_lightmapper_cpu() {
	return memnew(LightmapperCPU);
}
#endif

void register_lightmapper_cpu_types() {
	GLOBAL_DEF("rendering/cpu_lightmapper/quality/low_quality_ray_count", 64);
	GLOBAL_DEF("rendering/cpu_lightmapper/quality/medium_quality_ray_count", 256);
	GLOBAL_DEF("rendering/cpu_lightmapper/quality/high_quality_ray_count", 512);
	GLOBAL_DEF("rendering/cpu_lightmapper/quality/ultra_quality_ray_count", 1024);
#ifndef _3D_DISABLED
	Lightmapper::create_cpu = create_lightmapper_cpu;
#endif
}

void unregister_lightmapper_cpu_types() {
}
