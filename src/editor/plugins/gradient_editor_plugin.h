/**************************************************************************/
/*  gradient_editor_plugin.h                                              */
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

#ifndef GRADIENT_EDITOR_PLUGIN_H
#define GRADIENT_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/gui/gradient_edit.h"

class GradientEditor : public GradientEdit {
	GDCLASS(GradientEditor, GradientEdit);

	bool editing;
	Ref<Gradient> gradient;

	void _gradient_changed();
	void _ramp_changed();

protected:
	static void _bind_methods();

public:
	virtual Size2 get_minimum_size() const;
	void set_gradient(const Ref<Gradient> &p_gradient);
	GradientEditor();
};

class EditorInspectorPluginGradient : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginGradient, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class GradientEditorPlugin : public EditorPlugin {
	GDCLASS(GradientEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const { return "ColorRamp"; }

	GradientEditorPlugin(EditorNode *p_node);
};

#endif // GRADIENT_EDITOR_PLUGIN_H
