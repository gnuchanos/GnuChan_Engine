/**************************************************************************/
/*  skeleton_editor_plugin.h                                              */
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

#ifndef SKELETON_EDITOR_PLUGIN_H
#define SKELETON_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/skeleton.h"

class PhysicalBone;
class Joint;

class SkeletonEditor : public Node {
	GDCLASS(SkeletonEditor, Node);

	enum Menu {
		MENU_OPTION_CREATE_PHYSICAL_SKELETON
	};

	struct BoneInfo {
		PhysicalBone *physical_bone;
		Transform relative_rest; // Relative to skeleton node
		BoneInfo() :
				physical_bone(nullptr) {}
	};

	Skeleton *skeleton;

	MenuButton *options;

	void _on_click_option(int p_option);

	friend class SkeletonEditorPlugin;

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

	void create_physical_skeleton();
	PhysicalBone *create_physical_bone(int bone_id, int bone_child_id, const Vector<BoneInfo> &bones_infos);

public:
	void edit(Skeleton *p_node);

	SkeletonEditor();
	~SkeletonEditor();
};

class SkeletonEditorPlugin : public EditorPlugin {
	GDCLASS(SkeletonEditorPlugin, EditorPlugin);

	EditorNode *editor;
	SkeletonEditor *skeleton_editor;

public:
	virtual String get_name() const { return "Skeleton"; }
	virtual bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	SkeletonEditorPlugin(EditorNode *p_node);
	~SkeletonEditorPlugin();
};

#endif // SKELETON_EDITOR_PLUGIN_H
