/**************************************************************************/
/*  remote_transform_2d.h                                                 */
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

#ifndef REMOTE_TRANSFORM_2D_H
#define REMOTE_TRANSFORM_2D_H

#include "scene/2d/node_2d.h"

class RemoteTransform2D : public Node2D {
	GDCLASS(RemoteTransform2D, Node2D);

	NodePath remote_node;

	ObjectID cache;

	bool use_global_coordinates;
	bool update_remote_position;
	bool update_remote_rotation;
	bool update_remote_scale;

	void _update_remote();
	void _update_cache();
	//void _node_exited_scene();
protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_remote_node(const NodePath &p_remote_node);
	NodePath get_remote_node() const;

	void set_use_global_coordinates(const bool p_enable);
	bool get_use_global_coordinates() const;

	void set_update_position(const bool p_update);
	bool get_update_position() const;

	void set_update_rotation(const bool p_update);
	bool get_update_rotation() const;

	void set_update_scale(const bool p_update);
	bool get_update_scale() const;

	void force_update_cache();

	virtual String get_configuration_warning() const;

	RemoteTransform2D();
};

#endif // REMOTE_TRANSFORM_2D_H
