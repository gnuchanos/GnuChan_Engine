/**************************************************************************/
/*  gradient_editor_plugin.cpp                                            */
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

#include "gradient_editor_plugin.h"

#include "canvas_item_editor_plugin.h"
#include "editor/editor_scale.h"
#include "spatial_editor_plugin.h"

Size2 GradientEditor::get_minimum_size() const {
	return Size2(0, 60) * EDSCALE;
}
void GradientEditor::_gradient_changed() {
	if (editing) {
		return;
	}

	editing = true;
	Vector<Gradient::Point> points = gradient->get_points();
	set_points(points);
	set_interpolation_mode(gradient->get_interpolation_mode());
	update();
	editing = false;
}

void GradientEditor::_ramp_changed() {
	editing = true;
	UndoRedo *undo_redo = EditorNode::get_singleton()->get_undo_redo();
	undo_redo->create_action(TTR("Gradient Edited"), UndoRedo::MERGE_ENDS);
	undo_redo->add_do_method(gradient.ptr(), "set_offsets", get_offsets());
	undo_redo->add_do_method(gradient.ptr(), "set_colors", get_colors());
	undo_redo->add_do_method(gradient.ptr(), "set_interpolation_mode", get_interpolation_mode());
	undo_redo->add_undo_method(gradient.ptr(), "set_offsets", gradient->get_offsets());
	undo_redo->add_undo_method(gradient.ptr(), "set_colors", gradient->get_colors());
	undo_redo->add_undo_method(gradient.ptr(), "set_interpolation_mode", gradient->get_interpolation_mode());
	undo_redo->commit_action();
	editing = false;
}

void GradientEditor::_bind_methods() {
	ClassDB::bind_method("_gradient_changed", &GradientEditor::_gradient_changed);
	ClassDB::bind_method("_ramp_changed", &GradientEditor::_ramp_changed);
}

void GradientEditor::set_gradient(const Ref<Gradient> &p_gradient) {
	gradient = p_gradient;
	connect("ramp_changed", this, "_ramp_changed");
	gradient->connect("changed", this, "_gradient_changed");
	set_points(gradient->get_points());
	set_interpolation_mode(gradient->get_interpolation_mode());
}

GradientEditor::GradientEditor() {
	editing = false;
}

///////////////////////

bool EditorInspectorPluginGradient::can_handle(Object *p_object) {
	return Object::cast_to<Gradient>(p_object) != nullptr;
}

void EditorInspectorPluginGradient::parse_begin(Object *p_object) {
	Gradient *gradient = Object::cast_to<Gradient>(p_object);
	Ref<Gradient> g(gradient);

	GradientEditor *editor = memnew(GradientEditor);
	editor->set_gradient(g);
	add_custom_control(editor);
}

GradientEditorPlugin::GradientEditorPlugin(EditorNode *p_node) {
	Ref<EditorInspectorPluginGradient> plugin;
	plugin.instance();
	add_inspector_plugin(plugin);
}
