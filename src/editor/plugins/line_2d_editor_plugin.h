/**************************************************************************/
/*  line_2d_editor_plugin.h                                               */
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

#ifndef LINE_2D_EDITOR_PLUGIN_H
#define LINE_2D_EDITOR_PLUGIN_H

#include "editor/plugins/abstract_polygon_2d_editor.h"
#include "scene/2d/line_2d.h"

class Line2DEditor : public AbstractPolygon2DEditor {
	GDCLASS(Line2DEditor, AbstractPolygon2DEditor);

	Line2D *node;

protected:
	virtual Node2D *_get_node() const;
	virtual void _set_node(Node *p_line);

	virtual bool _is_line() const;
	virtual Variant _get_polygon(int p_idx) const;
	virtual void _set_polygon(int p_idx, const Variant &p_polygon) const;
	virtual void _action_set_polygon(int p_idx, const Variant &p_previous, const Variant &p_polygon);

public:
	Line2DEditor(EditorNode *p_editor);
};

class Line2DEditorPlugin : public AbstractPolygon2DEditorPlugin {
	GDCLASS(Line2DEditorPlugin, AbstractPolygon2DEditorPlugin);

public:
	Line2DEditorPlugin(EditorNode *p_node);
};

#endif // LINE_2D_EDITOR_PLUGIN_H
