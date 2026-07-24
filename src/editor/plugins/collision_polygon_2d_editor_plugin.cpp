/**************************************************************************/
/*  collision_polygon_2d_editor_plugin.cpp                                */
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

#include "collision_polygon_2d_editor_plugin.h"

Node2D *CollisionPolygon2DEditor::_get_node() const {
	return node;
}

void CollisionPolygon2DEditor::_set_node(Node *p_polygon) {
	node = Object::cast_to<CollisionPolygon2D>(p_polygon);
}

CollisionPolygon2DEditor::CollisionPolygon2DEditor(EditorNode *p_editor) :
		AbstractPolygon2DEditor(p_editor) {
	node = nullptr;
}

CollisionPolygon2DEditorPlugin::CollisionPolygon2DEditorPlugin(EditorNode *p_node) :
		AbstractPolygon2DEditorPlugin(p_node, memnew(CollisionPolygon2DEditor(p_node)), "CollisionPolygon2D") {
}
