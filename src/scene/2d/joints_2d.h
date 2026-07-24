/**************************************************************************/
/*  joints_2d.h                                                           */
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

#ifndef JOINTS_2D_H
#define JOINTS_2D_H

#include "node_2d.h"

class PhysicsBody2D;

class Joint2D : public Node2D {
	GDCLASS(Joint2D, Node2D);

	RID joint;
	RID ba, bb;

	NodePath a;
	NodePath b;
	real_t bias;

	bool exclude_from_collision;
	String warning;

protected:
	void _disconnect_signals();
	void _body_exit_tree();
	void _update_joint(bool p_only_free = false);

	void _notification(int p_what);
	virtual RID _configure_joint(PhysicsBody2D *body_a, PhysicsBody2D *body_b) = 0;

	static void _bind_methods();

public:
	virtual String get_configuration_warning() const;

	void set_node_a(const NodePath &p_node_a);
	NodePath get_node_a() const;

	void set_node_b(const NodePath &p_node_b);
	NodePath get_node_b() const;

	void set_bias(real_t p_bias);
	real_t get_bias() const;

	void set_exclude_nodes_from_collision(bool p_enable);
	bool get_exclude_nodes_from_collision() const;

	RID get_joint() const { return joint; }
	Joint2D();
};

class PinJoint2D : public Joint2D {
	GDCLASS(PinJoint2D, Joint2D);

	real_t softness;

protected:
	void _notification(int p_what);
	virtual RID _configure_joint(PhysicsBody2D *body_a, PhysicsBody2D *body_b);
	static void _bind_methods();

public:
	void set_softness(real_t p_softness);
	real_t get_softness() const;

	PinJoint2D();
};

class GrooveJoint2D : public Joint2D {
	GDCLASS(GrooveJoint2D, Joint2D);

	real_t length;
	real_t initial_offset;

protected:
	void _notification(int p_what);
	virtual RID _configure_joint(PhysicsBody2D *body_a, PhysicsBody2D *body_b);
	static void _bind_methods();

public:
	void set_length(real_t p_length);
	real_t get_length() const;

	void set_initial_offset(real_t p_initial_offset);
	real_t get_initial_offset() const;

	GrooveJoint2D();
};

class DampedSpringJoint2D : public Joint2D {
	GDCLASS(DampedSpringJoint2D, Joint2D);

	real_t stiffness;
	real_t damping;
	real_t rest_length;
	real_t length;

protected:
	void _notification(int p_what);
	virtual RID _configure_joint(PhysicsBody2D *body_a, PhysicsBody2D *body_b);
	static void _bind_methods();

public:
	void set_length(real_t p_length);
	real_t get_length() const;

	void set_rest_length(real_t p_rest_length);
	real_t get_rest_length() const;

	void set_damping(real_t p_damping);
	real_t get_damping() const;

	void set_stiffness(real_t p_stiffness);
	real_t get_stiffness() const;

	DampedSpringJoint2D();
};

#endif // JOINTS_2D_H
