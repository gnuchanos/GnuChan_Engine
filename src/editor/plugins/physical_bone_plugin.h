/**************************************************************************/
/*  physical_bone_plugin.h                                                */
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

#ifndef PHYSICAL_BONE_PLUGIN_H
#define PHYSICAL_BONE_PLUGIN_H

#include "editor/editor_node.h"

class PhysicalBoneEditor : public Object {
	GDCLASS(PhysicalBoneEditor, Object);

	EditorNode *editor;
	HBoxContainer *spatial_editor_hb;
	ToolButton *button_transform_joint;

	PhysicalBone *selected;

protected:
	static void _bind_methods();

private:
	void _on_toggle_button_transform_joint(bool p_is_pressed);
	void _set_move_joint();

public:
	PhysicalBoneEditor(EditorNode *p_editor);
	~PhysicalBoneEditor();

	void set_selected(PhysicalBone *p_pb);

	void hide();
	void show();
};

class PhysicalBonePlugin : public EditorPlugin {
	GDCLASS(PhysicalBonePlugin, EditorPlugin);

	EditorNode *editor;
	PhysicalBone *selected;
	PhysicalBoneEditor physical_bone_editor;

public:
	virtual String get_name() const { return "PhysicalBone"; }
	virtual bool handles(Object *p_object) const { return p_object->is_class("PhysicalBone"); }
	virtual void make_visible(bool p_visible);
	virtual void edit(Object *p_node);

	PhysicalBonePlugin(EditorNode *p_editor);
};

#endif // PHYSICAL_BONE_PLUGIN_H
