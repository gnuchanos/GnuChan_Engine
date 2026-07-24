/**************************************************************************/
/*  visual_server_constants.h                                             */
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

#ifndef VISUAL_SERVER_CONSTANTS_H
#define VISUAL_SERVER_CONSTANTS_H

// Use for constants etc that need not be included as often as VisualServer.h
// to reduce dependencies and prevent slow compilation.

// This is a "cheap" include, and can be used from scene side code as well as servers.

// Uncomment to provide comparison of node culling versus item culling
// #define VISUAL_SERVER_CANVAS_TIME_NODE_CULLING

// N.B. ONLY allow these defined in DEV_ENABLED builds, they will slow
// performance, and are only necessary to use for debugging.
#ifdef DEV_ENABLED

// Uncomment this define to store canvas item names in VisualServerCanvas.
// This is relatively expensive, but is invaluable for debugging the canvas scene tree
// especially using _print_tree() in VisualServerCanvas.
// #define VISUAL_SERVER_CANVAS_DEBUG_ITEM_NAMES

// Uncomment this define to verify local bounds of canvas items,
// to check that the hierarchical culling is working correctly.
// This is expensive.
// #define VISUAL_SERVER_CANVAS_CHECK_BOUNDS

// Uncomment this define to produce debugging output for physics interpolation.
// #define VISUAL_SERVER_DEBUG_PHYSICS_INTERPOLATION

#endif // DEV_ENABLED

#endif // VISUAL_SERVER_CONSTANTS_H
