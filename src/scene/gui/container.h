/**************************************************************************/
/*  container.h                                                           */
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

#ifndef CONTAINER_H
#define CONTAINER_H

#include "scene/gui/control.h"

class Container : public Control {
	GDCLASS(Container, Control);

	bool pending_sort;
	void _sort_children();
	void _child_minsize_changed();

protected:
	void queue_sort();
	virtual void add_child_notify(Node *p_child);
	virtual void move_child_notify(Node *p_child);
	virtual void remove_child_notify(Node *p_child);

	void _notification(int p_what);
	static void _bind_methods();

public:
	enum {
		NOTIFICATION_SORT_CHILDREN = 50
	};

	void fit_child_in_rect(Control *p_child, const Rect2 &p_rect);

	virtual String get_configuration_warning() const;

	Container();
};

#endif // CONTAINER_H
