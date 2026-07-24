/**************************************************************************/
/*  octree.h                                                              */
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

#ifndef OCTREE_H
#define OCTREE_H

#define OCTREE_ELEMENT_INVALID_ID 0
#define OCTREE_SIZE_LIMIT 1e15
#define OCTREE_DEFAULT_OCTANT_LIMIT 0

// We want 2 versions of the octree, Octree
// and Octree_CL which uses cached lists (optimized).
// we don't want to use the extra memory of cached lists on
// the non cached list version, so we use macros
// to avoid duplicating the code which is in octree_definition.
// The name of the class is overridden and the changes with the define
// OCTREE_USE_CACHED_LISTS.

// The two classes can be used identically but one contains the cached
// list optimization.

// standard octree
#define OCTREE_CLASS_NAME Octree
#undef OCTREE_USE_CACHED_LISTS
#include "octree_definition.inc"
#undef OCTREE_CLASS_NAME

// cached lists octree
#define OCTREE_CLASS_NAME Octree_CL
#define OCTREE_USE_CACHED_LISTS
#include "octree_definition.inc"
#undef OCTREE_CLASS_NAME

#endif // OCTREE_H
