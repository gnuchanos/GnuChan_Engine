/**************************************************************************/
/*  multimesh_editor_plugin.h                                             */
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

#ifndef MULTIMESH_EDITOR_PLUGIN_H
#define MULTIMESH_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/multimesh_instance.h"
#include "scene/gui/spin_box.h"

class MultiMeshEditor : public Control {
	GDCLASS(MultiMeshEditor, Control);

	friend class MultiMeshEditorPlugin;

	AcceptDialog *err_dialog;
	MenuButton *options;
	MultiMeshInstance *_last_pp_node;
	bool browsing_source;

	Panel *panel;
	MultiMeshInstance *node;

	LineEdit *surface_source;
	LineEdit *mesh_source;

	SceneTreeDialog *std;

	ConfirmationDialog *populate_dialog;
	OptionButton *populate_axis;
	HSlider *populate_rotate_random;
	HSlider *populate_tilt_random;
	SpinBox *populate_scale_random;
	SpinBox *populate_scale;
	SpinBox *populate_amount;

	enum Menu {

		MENU_OPTION_POPULATE
	};

	void _browsed(const NodePath &p_path);
	void _menu_option(int);
	void _populate();
	void _browse(bool p_source);

protected:
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	void edit(MultiMeshInstance *p_multimesh);
	MultiMeshEditor();
};

class MultiMeshEditorPlugin : public EditorPlugin {
	GDCLASS(MultiMeshEditorPlugin, EditorPlugin);

	MultiMeshEditor *multimesh_editor;
	EditorNode *editor;

public:
	virtual String get_name() const { return "MultiMesh"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	MultiMeshEditorPlugin(EditorNode *p_node);
	~MultiMeshEditorPlugin();
};

#endif // MULTIMESH_EDITOR_PLUGIN_H
