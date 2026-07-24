/**************************************************************************/
/*  lightmapper.cpp                                                       */
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

#include "lightmapper.h"

LightmapDenoiser *(*LightmapDenoiser::create_function)() = nullptr;

Ref<LightmapDenoiser> LightmapDenoiser::create() {
	if (create_function) {
		return Ref<LightmapDenoiser>(create_function());
	}
	return Ref<LightmapDenoiser>();
}

LightmapRaycaster *(*LightmapRaycaster::create_function)() = nullptr;

Ref<LightmapRaycaster> LightmapRaycaster::create() {
	if (create_function) {
		return Ref<LightmapRaycaster>(create_function());
	}
	return Ref<LightmapRaycaster>();
}

Lightmapper::CreateFunc Lightmapper::create_custom = nullptr;
Lightmapper::CreateFunc Lightmapper::create_gpu = nullptr;
Lightmapper::CreateFunc Lightmapper::create_cpu = nullptr;

Ref<Lightmapper> Lightmapper::create() {
	Lightmapper *lm = nullptr;
	if (create_custom) {
		lm = create_custom();
	}

	if (!lm && create_gpu) {
		lm = create_gpu();
	}

	if (!lm && create_cpu) {
		lm = create_cpu();
	}
	if (!lm) {
		return Ref<Lightmapper>();
	} else {
		return Ref<Lightmapper>(lm);
	}
}

Lightmapper::Lightmapper() {
}
