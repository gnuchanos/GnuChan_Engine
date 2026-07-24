/**************************************************************************/
/*  collision_polygon_2d_editor_plugin.h                                  */
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

#ifndef COLLISION_POLYGON_2D_EDITOR_PLUGIN_H
#define COLLISION_POLYGON_2D_EDITOR_PLUGIN_H

#include "editor/plugins/abstract_polygon_2d_editor.h"
#include "scene/2d/collision_polygon_2d.h"

class CollisionPolygon2DEditor : public AbstractPolygon2DEditor {
	GDCLASS(CollisionPolygon2DEditor, AbstractPolygon2DEditor);

	CollisionPolygon2D *node;

protected:
	virtual Node2D *_get_node() const;
	virtual void _set_node(Node *p_polygon);

public:
	CollisionPolygon2DEditor(EditorNode *p_editor);
};

class CollisionPolygon2DEditorPlugin : public AbstractPolygon2DEditorPlugin {
	GDCLASS(CollisionPolygon2DEditorPlugin, AbstractPolygon2DEditorPlugin);

public:
	CollisionPolygon2DEditorPlugin(EditorNode *p_node);
};

#endif // COLLISION_POLYGON_2D_EDITOR_PLUGIN_H
