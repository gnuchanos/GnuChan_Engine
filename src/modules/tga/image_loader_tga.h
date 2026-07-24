/**************************************************************************/
/*  image_loader_tga.h                                                    */
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

#ifndef IMAGE_LOADER_TGA_H
#define IMAGE_LOADER_TGA_H

#include "core/io/image_loader.h"

/**
	@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>
*/
#define TGA_IMAGE_DESCRIPTOR_ALPHA_MASK 0xf

class ImageLoaderTGA : public ImageFormatLoader {
	enum tga_type_e {
		TGA_TYPE_NO_DATA = 0,
		TGA_TYPE_INDEXED = 1,
		TGA_TYPE_RGB = 2,
		TGA_TYPE_MONOCHROME = 3,
		TGA_TYPE_RLE_INDEXED = 9,
		TGA_TYPE_RLE_RGB = 10,
		TGA_TYPE_RLE_MONOCHROME = 11
	};

	enum tga_origin_e {
		TGA_ORIGIN_BOTTOM_LEFT = 0x00,
		TGA_ORIGIN_BOTTOM_RIGHT = 0x01,
		TGA_ORIGIN_TOP_LEFT = 0x02,
		TGA_ORIGIN_TOP_RIGHT = 0x03,
		TGA_ORIGIN_SHIFT = 0x04,
		TGA_ORIGIN_MASK = 0x30
	};

	struct tga_header_s {
		uint8_t id_length;
		uint8_t color_map_type;
		tga_type_e image_type;

		uint16_t first_color_entry;
		uint16_t color_map_length;
		uint8_t color_map_depth;

		uint16_t x_origin;
		uint16_t y_origin;
		uint16_t image_width;
		uint16_t image_height;
		uint8_t pixel_depth;
		uint8_t image_descriptor;
	};
	static Error decode_tga_rle(const uint8_t *p_compressed_buffer, size_t p_pixel_size, uint8_t *p_uncompressed_buffer, size_t p_output_size, size_t p_input_size);
	static Error convert_to_image(Ref<Image> p_image, const uint8_t *p_buffer, const tga_header_s &p_header, const uint8_t *p_palette, const bool p_is_monochrome, size_t p_input_size);

public:
	virtual Error load_image(Ref<Image> p_image, FileAccess *f, bool p_force_linear, float p_scale);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	ImageLoaderTGA();
};

#endif // IMAGE_LOADER_TGA_H
