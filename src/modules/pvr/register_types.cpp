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

#include "image_compress_pvrtc.h"
#include "texture_loader_pvr.h"

static Ref<ResourceFormatPVR> resource_loader_pvr;

void register_pvr_types() {
	resource_loader_pvr.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_pvr);

	_register_pvrtc_compress_func();
}

void unregister_pvr_types() {
	ResourceLoader::remove_resource_format_loader(resource_loader_pvr);
	resource_loader_pvr.unref();
}
