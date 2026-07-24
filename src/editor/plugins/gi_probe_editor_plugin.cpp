/**************************************************************************/
/*  gi_probe_editor_plugin.cpp                                            */
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

#include "gi_probe_editor_plugin.h"

void GIProbeEditorPlugin::_bake() {
	if (gi_probe) {
		gi_probe->bake();
	}
}

void GIProbeEditorPlugin::edit(Object *p_object) {
	GIProbe *s = Object::cast_to<GIProbe>(p_object);
	if (!s) {
		return;
	}

	gi_probe = s;
}

bool GIProbeEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("GIProbe");
}

void GIProbeEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		bake->show();
	} else {
		bake->hide();
	}
}

EditorProgress *GIProbeEditorPlugin::tmp_progress = nullptr;

void GIProbeEditorPlugin::bake_func_begin(int p_steps) {
	ERR_FAIL_COND(tmp_progress != nullptr);

	tmp_progress = memnew(EditorProgress("bake_gi", TTR("Bake GI Probe"), p_steps));
}

void GIProbeEditorPlugin::bake_func_step(int p_step, const String &p_description) {
	ERR_FAIL_COND(tmp_progress == nullptr);
	tmp_progress->step(p_description, p_step, false);
}

void GIProbeEditorPlugin::bake_func_end() {
	ERR_FAIL_COND(tmp_progress == nullptr);
	memdelete(tmp_progress);
	tmp_progress = nullptr;
}

void GIProbeEditorPlugin::_bind_methods() {
	ClassDB::bind_method("_bake", &GIProbeEditorPlugin::_bake);
}

GIProbeEditorPlugin::GIProbeEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	bake = memnew(ToolButton);
	bake->set_icon(editor->get_gui_base()->get_icon("Bake", "EditorIcons"));
	bake->set_text(TTR("Bake GI Probe"));
	bake->hide();
	bake->connect("pressed", this, "_bake");
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, bake);
	gi_probe = nullptr;

	GIProbe::bake_begin_function = bake_func_begin;
	GIProbe::bake_step_function = bake_func_step;
	GIProbe::bake_end_function = bake_func_end;
}

GIProbeEditorPlugin::~GIProbeEditorPlugin() {
}
