/**************************************************************************/
/*  editor_folding.h                                                      */
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

#ifndef EDITOR_FOLDING_H
#define EDITOR_FOLDING_H

#include "scene/main/node.h"

class EditorFolding {
	PoolVector<String> _get_unfolds(const Object *p_object);
	void _set_unfolds(Object *p_object, const PoolVector<String> &p_unfolds);

	void _fill_folds(const Node *p_root, const Node *p_node, Array &p_folds, Array &resource_folds, Array &nodes_folded, Set<RES> &resources);

	void _do_object_unfolds(Object *p_object, Set<RES> &resources);
	void _do_node_unfolds(Node *p_root, Node *p_node, Set<RES> &resources);

public:
	void save_resource_folding(const RES &p_resource, const String &p_path);
	void load_resource_folding(RES p_resource, const String &p_path);

	void save_scene_folding(const Node *p_scene, const String &p_path);
	void load_scene_folding(Node *p_scene, const String &p_path);

	void unfold_scene(Node *p_scene);

	bool has_folding_data(const String &p_path);

	EditorFolding();
};

#endif // EDITOR_FOLDING_H
