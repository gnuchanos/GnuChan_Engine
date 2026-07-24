/**************************************************************************/
/*  style_box_editor_plugin.h                                             */
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

#ifndef STYLE_BOX_EDITOR_PLUGIN_H
#define STYLE_BOX_EDITOR_PLUGIN_H

#include "editor/editor_inspector.h"
#include "editor/editor_node.h"
#include "scene/gui/option_button.h"
#include "scene/gui/texture_rect.h"
#include "scene/resources/style_box.h"

class StyleBoxPreview : public VBoxContainer {
	GDCLASS(StyleBoxPreview, VBoxContainer);

	Control *preview;
	Ref<StyleBox> stylebox;

	void _sb_changed();
	void _redraw();

protected:
	static void _bind_methods();

public:
	void edit(const Ref<StyleBox> &p_stylebox);

	StyleBoxPreview();
};

class EditorInspectorPluginStyleBox : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginStyleBox, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
	virtual bool parse_property(Object *p_object, Variant::Type p_type, const String &p_path, PropertyHint p_hint, const String &p_hint_text, int p_usage);
	virtual void parse_end();
};

class StyleBoxEditorPlugin : public EditorPlugin {
	GDCLASS(StyleBoxEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const { return "StyleBox"; }

	StyleBoxEditorPlugin(EditorNode *p_node);
};

#endif // STYLE_BOX_EDITOR_PLUGIN_H
