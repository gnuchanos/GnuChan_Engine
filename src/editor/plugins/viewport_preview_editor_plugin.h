/**************************************************************************/
/*  viewport_preview_editor_plugin.h                                      */
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

#ifndef VIEWPORT_PREVIEW_EDITOR_PLUGIN_H
#define VIEWPORT_PREVIEW_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/plugins/texture_editor_plugin.h"
#include "scene/main/viewport.h"

class EditorInspectorPluginViewportPreview : public EditorInspectorPluginTexture {
	GDCLASS(EditorInspectorPluginViewportPreview, EditorInspectorPluginTexture);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class ViewportPreviewEditorPlugin : public EditorPlugin {
	GDCLASS(ViewportPreviewEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const { return "SubViewportPreview"; }

	ViewportPreviewEditorPlugin(EditorNode *p_node);
};

#endif // VIEWPORT_PREVIEW_EDITOR_PLUGIN_H
