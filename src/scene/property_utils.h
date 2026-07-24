/**************************************************************************/
/*  property_utils.h                                                      */
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

#ifndef PROPERTY_UTILS_H
#define PROPERTY_UTILS_H

#include "scene/main/node.h"
#include "scene/resources/packed_scene.h"

class PropertyUtils {
public:
	static bool is_property_value_different(const Variant &p_a, const Variant &p_b);
	// Gets the most pure default value, the one that would be set when the node has just been instantiated
	static Variant get_property_default_value(const Object *p_object, const StringName &p_property, bool *r_is_valid = nullptr, const Vector<SceneState::PackState> *p_states_stack_cache = nullptr, bool p_update_exports = false, const Node *p_owner = nullptr, bool *r_is_class_default = nullptr);

	// Gets the instance/inheritance states of this node, in order of precedence,
	// that is, from the topmost (the most able to override values) to the lowermost
	// (Note that in nested instancing the one with the greatest precedence is the furthest
	// in the tree, since every owner found while traversing towards the root gets a chance
	// to override property values.)
	static Vector<SceneState::PackState> get_node_states_stack(const Node *p_node, const Node *p_owner = nullptr, bool *r_instanced_by_owner = nullptr);
};

#endif // PROPERTY_UTILS_H
