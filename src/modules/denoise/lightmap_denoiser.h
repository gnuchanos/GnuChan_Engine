/**************************************************************************/
/*  lightmap_denoiser.h                                                   */
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

#ifndef LIGHTMAP_DENOISER_H
#define LIGHTMAP_DENOISER_H

#include "core/class_db.h"
#include "scene/3d/lightmapper.h"

struct OIDNDeviceImpl;

class LightmapDenoiserOIDN : public LightmapDenoiser {
	GDCLASS(LightmapDenoiserOIDN, LightmapDenoiser);

protected:
	void *device = nullptr;

public:
	static LightmapDenoiser *create_oidn_denoiser();

	Ref<Image> denoise_image(const Ref<Image> &p_image);

	static void make_default_denoiser();

	LightmapDenoiserOIDN();
	~LightmapDenoiserOIDN();
};

#endif // LIGHTMAP_DENOISER_H
