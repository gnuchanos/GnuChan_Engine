/**************************************************************************/
/*  lightmap_denoiser.cpp                                                 */
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

#include "lightmap_denoiser.h"

#include "denoise_wrapper.h"

LightmapDenoiser *LightmapDenoiserOIDN::create_oidn_denoiser() {
	return memnew(LightmapDenoiserOIDN);
}

void LightmapDenoiserOIDN::make_default_denoiser() {
	create_function = create_oidn_denoiser;
}

Ref<Image> LightmapDenoiserOIDN::denoise_image(const Ref<Image> &p_image) {
	Ref<Image> img = p_image->duplicate();

	img->convert(Image::FORMAT_RGBF);

	PoolByteArray data = img->get_data();
	{
		PoolByteArray::Write w = data.write();
		if (!oidn_denoise(device, (float *)w.ptr(), img->get_width(), img->get_height())) {
			return p_image;
		}
	}

	img->create(img->get_width(), img->get_height(), false, img->get_format(), data);
	return img;
}

LightmapDenoiserOIDN::LightmapDenoiserOIDN() {
	device = oidn_denoiser_init();
}

LightmapDenoiserOIDN::~LightmapDenoiserOIDN() {
	oidn_denoiser_finish(device);
}
