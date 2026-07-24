/**************************************************************************/
/*  fbx_anim_container.h                                                  */
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

#ifndef FBX_ANIM_CONTAINER_H
#define FBX_ANIM_CONTAINER_H

#include "core/vector.h"

// Generic keyframes 99.99 percent of files will be vector3, except if quat interp is used, or visibility tracks
// FBXTrack is used in a map in the implementation in fbx/editor_scene_importer_fbx.cpp
// to avoid having to rewrite the entire logic I refactored this into the code instead.
// once it works I can rewrite so we can add the fun misc features / small features
struct FBXTrack {
	bool has_default = false;
	Vector3 default_value;
	std::map<int64_t, Vector3> keyframes;
};

#endif // FBX_ANIM_CONTAINER_H
