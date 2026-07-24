/**************************************************************************/
/*  register_driver_types.cpp                                             */
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

#include "register_driver_types.h"

#include "drivers/png/image_loader_png.h"
#include "drivers/png/resource_saver_png.h"

static ImageLoaderPNG *image_loader_png;
static Ref<ResourceSaverPNG> resource_saver_png;

void register_core_driver_types() {
	image_loader_png = memnew(ImageLoaderPNG);
	ImageLoader::add_image_format_loader(image_loader_png);

	resource_saver_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_png);
}

void unregister_core_driver_types() {
	if (image_loader_png) {
		memdelete(image_loader_png);
	}

	ResourceSaver::remove_resource_format_saver(resource_saver_png);
	resource_saver_png.unref();
}

void register_driver_types() {
}

void unregister_driver_types() {
}
