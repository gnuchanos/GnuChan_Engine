/**************************************************************************/
/*  csg_gizmos.h                                                          */
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

#ifndef CSG_GIZMOS_H
#define CSG_GIZMOS_H

#include "csg_shape.h"
#include "editor/editor_plugin.h"
#include "editor/spatial_editor_gizmos.h"

class CSGShapeSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(CSGShapeSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	bool has_gizmo(Spatial *p_spatial);
	String get_name() const;
	int get_priority() const;
	bool is_selectable_when_hidden() const;
	void redraw(EditorSpatialGizmo *p_gizmo);

	String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const;
	Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const;
	void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point);
	void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel);

	CSGShapeSpatialGizmoPlugin();
};

class EditorPluginCSG : public EditorPlugin {
	GDCLASS(EditorPluginCSG, EditorPlugin);

public:
	EditorPluginCSG(EditorNode *p_editor);
};

#endif // CSG_GIZMOS_H
