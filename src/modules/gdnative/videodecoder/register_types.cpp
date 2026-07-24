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

#include "core/class_db.h"
#include "video_stream_gdnative.h"

static Ref<ResourceFormatLoaderVideoStreamGDNative> resource_loader_vsgdnative;

void register_videodecoder_types() {
	resource_loader_vsgdnative.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_vsgdnative, true);

	ClassDB::register_class<VideoStreamGDNative>();
}

void unregister_videodecoder_types() {
	ResourceLoader::remove_resource_format_loader(resource_loader_vsgdnative);
	resource_loader_vsgdnative.unref();
}
