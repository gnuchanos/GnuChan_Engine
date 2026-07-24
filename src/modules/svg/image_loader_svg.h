/**************************************************************************/
/*  image_loader_svg.h                                                    */
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

#ifndef IMAGE_LOADER_SVG_H
#define IMAGE_LOADER_SVG_H

#include "core/io/image_loader.h"
#include "core/ustring.h"

/**
	@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>
*/

// Forward declare and include thirdparty headers in .cpp.
struct NSVGrasterizer;
struct NSVGimage;

class SVGRasterizer {
	NSVGrasterizer *rasterizer;

public:
	void rasterize(NSVGimage *p_image, float p_tx, float p_ty, float p_scale, unsigned char *p_dst, int p_w, int p_h, int p_stride);

	SVGRasterizer();
	~SVGRasterizer();
};

class ImageLoaderSVG : public ImageFormatLoader {
	static struct ReplaceColors {
		List<uint32_t> old_colors;
		List<uint32_t> new_colors;
	} replace_colors;
	static SVGRasterizer rasterizer;
	static void _convert_colors(NSVGimage *p_svg_image);

public:
	static void set_convert_colors(Dictionary *p_replace_color = nullptr);
	static Error create_image(Ref<Image> p_image, const PoolVector<uint8_t> *p_data, float p_scale, bool upsample, bool convert_colors = false);
	static Error create_image_from_string(Ref<Image> p_image, const char *p_svg_str, float p_scale, bool upsample, bool convert_colors = false);

	virtual Error load_image(Ref<Image> p_image, FileAccess *f, bool p_force_linear, float p_scale);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	ImageLoaderSVG();
};

#endif // IMAGE_LOADER_SVG_H
