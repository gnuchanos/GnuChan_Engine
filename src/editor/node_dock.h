/**************************************************************************/
/*  node_dock.h                                                           */
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

#ifndef NODE_DOCK_H
#define NODE_DOCK_H

#include "connections_dialog.h"
#include "groups_editor.h"

class NodeDock : public VBoxContainer {
	GDCLASS(NodeDock, VBoxContainer);

	ToolButton *connections_button;
	ToolButton *groups_button;

	ConnectionsDock *connections;
	GroupsEditor *groups;

	HBoxContainer *mode_hb;

	Label *select_a_node;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	static NodeDock *singleton;

	void set_node(Node *p_node);

	void show_groups();
	void show_connections();

	void update_lists();

	NodeDock();
};

#endif // NODE_DOCK_H
