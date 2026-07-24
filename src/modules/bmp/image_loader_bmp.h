/**************************************************************************/
/*  image_loader_bmp.h                                                    */
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

#ifndef IMAGE_LOADER_BMP_H
#define IMAGE_LOADER_BMP_H

#include "core/io/image_loader.h"

class ImageLoaderBMP : public ImageFormatLoader {
protected:
	static const unsigned BITMAP_SIGNATURE = 0x4d42;

	static const unsigned BITMAP_FILE_HEADER_SIZE = 14; // bmp_file_header_s
	static const unsigned BITMAP_INFO_HEADER_MIN_SIZE = 40; // bmp_info_header_s

	enum bmp_compression_s {
		BI_RGB = 0x00,
		BI_RLE8 = 0x01, // compressed
		BI_RLE4 = 0x02, // compressed
		BI_BITFIELDS = 0x03,
		BI_JPEG = 0x04,
		BI_PNG = 0x05,
		BI_ALPHABITFIELDS = 0x06,
		BI_CMYK = 0x0b,
		BI_CMYKRLE8 = 0x0c, // compressed
		BI_CMYKRLE4 = 0x0d // compressed
	};

	struct bmp_header_s {
		struct bmp_file_header_s {
			uint16_t bmp_signature;
			uint32_t bmp_file_size;
			uint32_t bmp_file_padding;
			uint32_t bmp_file_offset;
		} bmp_file_header;

		struct bmp_info_header_s {
			uint32_t bmp_header_size;
			uint32_t bmp_width;
			uint32_t bmp_height;
			uint16_t bmp_planes;
			uint16_t bmp_bit_count;
			uint32_t bmp_compression;
			uint32_t bmp_size_image;
			uint32_t bmp_pixels_per_meter_x;
			uint32_t bmp_pixels_per_meter_y;
			uint32_t bmp_colors_used;
			uint32_t bmp_important_colors;
		} bmp_info_header;
	};

	static Error convert_to_image(Ref<Image> p_image,
			const uint8_t *p_buffer,
			const uint8_t *p_color_buffer,
			const uint32_t color_table_size,
			const bmp_header_s &p_header);

public:
	virtual Error load_image(Ref<Image> p_image, FileAccess *f,
			bool p_force_linear, float p_scale);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	ImageLoaderBMP();
};

#endif // IMAGE_LOADER_BMP_H
