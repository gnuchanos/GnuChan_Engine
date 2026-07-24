/**************************************************************************/
/*  visibility_notifier_2d.h                                              */
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

#ifndef VISIBILITY_NOTIFIER_2D_H
#define VISIBILITY_NOTIFIER_2D_H

#include "scene/2d/node_2d.h"

class Viewport;
class VisibilityNotifier2D : public Node2D {
	GDCLASS(VisibilityNotifier2D, Node2D);

	Set<Viewport *> viewports;

	Rect2 rect;

protected:
	friend struct SpatialIndexer2D;

	void _enter_viewport(Viewport *p_viewport);
	void _exit_viewport(Viewport *p_viewport);

	virtual void _screen_enter() {}
	virtual void _screen_exit() {}

	void _notification(int p_what);
	static void _bind_methods();

public:
#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
#endif

	void set_rect(const Rect2 &p_rect);
	Rect2 get_rect() const;

	bool is_on_screen() const;

	VisibilityNotifier2D();
};

class VisibilityEnabler2D : public VisibilityNotifier2D {
	GDCLASS(VisibilityEnabler2D, VisibilityNotifier2D);

public:
	enum Enabler {
		ENABLER_PAUSE_ANIMATIONS,
		ENABLER_FREEZE_BODIES,
		ENABLER_PAUSE_PARTICLES,
		ENABLER_PARENT_PROCESS,
		ENABLER_PARENT_PHYSICS_PROCESS,
		ENABLER_PAUSE_ANIMATED_SPRITES,
		ENABLER_MAX
	};

protected:
	virtual void _screen_enter();
	virtual void _screen_exit();

	bool visible;

	void _find_nodes(Node *p_node);

	Map<Node *, Variant> nodes;
	void _node_removed(Node *p_node);
	bool enabler[ENABLER_MAX];

	void _change_node_state(Node *p_node, bool p_enabled);

	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_enabler(Enabler p_enabler, bool p_enable);
	bool is_enabler_enabled(Enabler p_enabler) const;

	String get_configuration_warning() const;

	VisibilityEnabler2D();
};

VARIANT_ENUM_CAST(VisibilityEnabler2D::Enabler);

#endif // VISIBILITY_NOTIFIER_2D_H
