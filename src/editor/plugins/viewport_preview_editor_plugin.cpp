/**************************************************************************/
/*  viewport_preview_editor_plugin.cpp                                    */
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

#include "viewport_preview_editor_plugin.h"

bool EditorInspectorPluginViewportPreview::can_handle(Object *p_object) {
	return Object::cast_to<Viewport>(p_object) != nullptr;
}

void EditorInspectorPluginViewportPreview::parse_begin(Object *p_object) {
	Viewport *viewport = Object::cast_to<Viewport>(p_object);

	TexturePreview *viewport_preview = memnew(TexturePreview(viewport->get_texture(), false));
	// Otherwise `viewport_preview`'s `texture_display` doesn't update properly when `viewport`'s size changes.
	viewport->connect("size_changed", viewport_preview->get_texture_display(), "update");
	add_custom_control(viewport_preview);
}

ViewportPreviewEditorPlugin::ViewportPreviewEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginViewportPreview> plugin;
	plugin.instance();
	add_inspector_plugin(plugin);
}
