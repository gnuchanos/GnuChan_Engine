/**************************************************************************/
/*  room_manager_editor_plugin.h                                          */
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

#ifndef ROOM_MANAGER_EDITOR_PLUGIN_H
#define ROOM_MANAGER_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "scene/3d/occluder.h"
#include "scene/3d/portal.h"
#include "scene/3d/room.h"
#include "scene/3d/room_manager.h"
#include "scene/resources/material.h"

class RoomManagerEditorPlugin : public EditorPlugin {
	GDCLASS(RoomManagerEditorPlugin, EditorPlugin);

	RoomManager *_room_manager;

	ToolButton *button_flip_portals;
	EditorNode *editor;

	void _flip_portals();

protected:
	static void _bind_methods();

public:
	virtual String get_name() const { return "RoomManager"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	RoomManagerEditorPlugin(EditorNode *p_node);
	~RoomManagerEditorPlugin();
};

///////////////////////

class RoomEditorPlugin : public EditorPlugin {
	GDCLASS(RoomEditorPlugin, EditorPlugin);

	Room *_room;
	ToolButton *button_generate;
	EditorNode *editor;
	UndoRedo *undo_redo;

	void _generate_points();

protected:
	static void _bind_methods();

public:
	virtual String get_name() const { return "Room"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	RoomEditorPlugin(EditorNode *p_node);
	~RoomEditorPlugin();
};

///////////////////////

class PortalEditorPlugin : public EditorPlugin {
	GDCLASS(PortalEditorPlugin, EditorPlugin);

	Portal *_portal;
	ToolButton *button_flip;
	EditorNode *editor;

	void _flip_portal();

protected:
	static void _bind_methods();

public:
	virtual String get_name() const { return "Portal"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	PortalEditorPlugin(EditorNode *p_node);
	~PortalEditorPlugin();
};

///////////////////////

class OccluderEditorPlugin : public EditorPlugin {
	GDCLASS(OccluderEditorPlugin, EditorPlugin);

	Occluder *_occluder;
	ToolButton *button_center;
	EditorNode *editor;
	UndoRedo *undo_redo;

	void _center();

protected:
	static void _bind_methods();

public:
	virtual String get_name() const { return "Occluder"; }
	bool has_main_screen() const { return false; }
	virtual void edit(Object *p_object);
	virtual bool handles(Object *p_object) const;
	virtual void make_visible(bool p_visible);

	OccluderEditorPlugin(EditorNode *p_node);
	~OccluderEditorPlugin();
};

#endif // ROOM_MANAGER_EDITOR_PLUGIN_H
