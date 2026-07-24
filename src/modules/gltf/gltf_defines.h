/**************************************************************************/
/*  gltf_defines.h                                                        */
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

#ifndef GLTF_DEFINES_H
#define GLTF_DEFINES_H

// This file should only be included by other headers.

// GnuChan classes used by GLTF headers.
class AnimationPlayer;
class BoneAttachment;
class CSGShape;
class DirectionalLight;
class GridMap;
class Light;
class MultiMeshInstance;
class Skeleton;
class Skin;

// GLTF classes.
struct GLTFAccessor;
class GLTFAnimation;
class GLTFBufferView;
class GLTFCamera;
class GLTFDocument;
class GLTFDocumentExtension;
class GLTFLight;
class GLTFMesh;
class GLTFNode;
class GLTFSkeleton;
class GLTFSkin;
class GLTFSpecGloss;
class GLTFState;
class GLTFTexture;
class GLTFTextureSampler;
class PackedSceneGLTF;

// GLTF index aliases.
using GLTFAccessorIndex = int;
using GLTFAnimationIndex = int;
using GLTFBufferIndex = int;
using GLTFBufferViewIndex = int;
using GLTFCameraIndex = int;
using GLTFImageIndex = int;
using GLTFLightIndex = int;
using GLTFMaterialIndex = int;
using GLTFMeshIndex = int;
using GLTFNodeIndex = int;
using GLTFSkeletonIndex = int;
using GLTFSkinIndex = int;
using GLTFTextureIndex = int;
using GLTFTextureSamplerIndex = int;

enum GLTFType {
	TYPE_SCALAR,
	TYPE_VEC2,
	TYPE_VEC3,
	TYPE_VEC4,
	TYPE_MAT2,
	TYPE_MAT3,
	TYPE_MAT4,
};

#endif // GLTF_DEFINES_H
