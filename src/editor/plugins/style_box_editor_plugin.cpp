/**************************************************************************/
/*  style_box_editor_plugin.cpp                                           */
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

#include "style_box_editor_plugin.h"

#include "editor/editor_scale.h"

bool EditorInspectorPluginStyleBox::can_handle(Object *p_object) {
	return Object::cast_to<StyleBox>(p_object) != nullptr;
}

void EditorInspectorPluginStyleBox::parse_begin(Object *p_object) {
	Ref<StyleBox> sb = Ref<StyleBox>(Object::cast_to<StyleBox>(p_object));

	StyleBoxPreview *preview = memnew(StyleBoxPreview);
	preview->edit(sb);
	add_custom_control(preview);
}
bool EditorInspectorPluginStyleBox::parse_property(Object *p_object, Variant::Type p_type, const String &p_path, PropertyHint p_hint, const String &p_hint_text, int p_usage) {
	return false; //do not want
}
void EditorInspectorPluginStyleBox::parse_end() {
}

void StyleBoxPreview::edit(const Ref<StyleBox> &p_stylebox) {
	if (stylebox.is_valid()) {
		stylebox->disconnect("changed", this, "_sb_changed");
	}
	stylebox = p_stylebox;
	if (p_stylebox.is_valid()) {
		preview->add_style_override("panel", stylebox);
		stylebox->connect("changed", this, "_sb_changed");
	}
	_sb_changed();
}

void StyleBoxPreview::_sb_changed() {
	preview->update();
}

void StyleBoxPreview::_redraw() {
	if (stylebox.is_valid()) {
		Rect2 preview_rect = preview->get_rect();

		// Re-adjust preview panel to fit all drawn content
		Rect2 draw_rect = stylebox->get_draw_rect(preview_rect);
		preview_rect.size -= draw_rect.size - preview_rect.size;
		preview_rect.position -= draw_rect.position - preview_rect.position;

		preview->draw_style_box(stylebox, preview_rect);
	}
}

void StyleBoxPreview::_bind_methods() {
	ClassDB::bind_method("_sb_changed", &StyleBoxPreview::_sb_changed);
	ClassDB::bind_method("_redraw", &StyleBoxPreview::_redraw);
}

StyleBoxPreview::StyleBoxPreview() {
	preview = memnew(Control);
	preview->set_custom_minimum_size(Size2(0, 150 * EDSCALE));
	preview->set_clip_contents(true);
	preview->connect("draw", this, "_redraw");
	add_margin_child(TTR("Preview:"), preview);
}

StyleBoxEditorPlugin::StyleBoxEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginStyleBox> inspector_plugin;
	inspector_plugin.instance();
	add_inspector_plugin(inspector_plugin);
}
