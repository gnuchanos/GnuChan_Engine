/**************************************************************************/
/*  skeleton_ik_editor_plugin.h                                           */
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

#ifndef SKELETON_IK_EDITOR_PLUGIN_H
#define SKELETON_IK_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"

class SkeletonIK;

class SkeletonIKEditorPlugin : public EditorPlugin {
	GDCLASS(SkeletonIKEditorPlugin, EditorPlugin);

	SkeletonIK *skeleton_ik;

	Button *play_btn;
	EditorNode *editor;

	void _play();

protected:
	static void _bind_methods();

public:
	virtual String get_name() const { return "SkeletonIK"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	SkeletonIKEditorPlugin(EditorNode *p_node);
	~SkeletonIKEditorPlugin();
};

#endif // SKELETON_IK_EDITOR_PLUGIN_H
