/**************************************************************************/
/*  node_path.cpp                                                         */
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

#include "gdnative/node_path.h"

#include "core/node_path.h"
#include "core/variant.h"

#ifdef __cplusplus
extern "C" {
#endif

static_assert(sizeof(godot_node_path) == sizeof(NodePath), "NodePath size mismatch");

void GDAPI godot_node_path_new(godot_node_path *r_dest, const godot_string *p_from) {
	NodePath *dest = (NodePath *)r_dest;
	const String *from = (const String *)p_from;
	memnew_placement(dest, NodePath(*from));
}

void GDAPI godot_node_path_new_copy(godot_node_path *r_dest, const godot_node_path *p_src) {
	NodePath *dest = (NodePath *)r_dest;
	const NodePath *src = (const NodePath *)p_src;
	memnew_placement(dest, NodePath(*src));
}

void GDAPI godot_node_path_destroy(godot_node_path *p_self) {
	NodePath *self = (NodePath *)p_self;
	self->~NodePath();
}

godot_string GDAPI godot_node_path_as_string(const godot_node_path *p_self) {
	godot_string ret;
	const NodePath *self = (const NodePath *)p_self;
	memnew_placement(&ret, String(*self));
	return ret;
}

godot_bool GDAPI godot_node_path_is_absolute(const godot_node_path *p_self) {
	const NodePath *self = (const NodePath *)p_self;
	return self->is_absolute();
}

godot_int GDAPI godot_node_path_get_name_count(const godot_node_path *p_self) {
	const NodePath *self = (const NodePath *)p_self;
	return self->get_name_count();
}

godot_string GDAPI godot_node_path_get_name(const godot_node_path *p_self, const godot_int p_idx) {
	godot_string dest;
	const NodePath *self = (const NodePath *)p_self;

	memnew_placement(&dest, String(self->get_name(p_idx)));
	return dest;
}

godot_int GDAPI godot_node_path_get_subname_count(const godot_node_path *p_self) {
	const NodePath *self = (const NodePath *)p_self;
	return self->get_subname_count();
}

godot_string GDAPI godot_node_path_get_subname(const godot_node_path *p_self, const godot_int p_idx) {
	godot_string dest;
	const NodePath *self = (const NodePath *)p_self;

	memnew_placement(&dest, String(self->get_subname(p_idx)));
	return dest;
}

godot_string GDAPI godot_node_path_get_concatenated_subnames(const godot_node_path *p_self) {
	godot_string dest;
	const NodePath *self = (const NodePath *)p_self;
	memnew_placement(&dest, String(self->get_concatenated_subnames()));
	return dest;
}

godot_bool GDAPI godot_node_path_is_empty(const godot_node_path *p_self) {
	const NodePath *self = (const NodePath *)p_self;
	return self->is_empty();
}

godot_bool GDAPI godot_node_path_operator_equal(const godot_node_path *p_self, const godot_node_path *p_b) {
	const NodePath *self = (const NodePath *)p_self;
	const NodePath *b = (const NodePath *)p_b;
	return *self == *b;
}

godot_node_path godot_node_path_get_as_property_path(const godot_node_path *p_self) {
	const NodePath *self = (const NodePath *)p_self;
	godot_node_path res;
	NodePath *val = (NodePath *)&res;
	memnew_placement(val, NodePath);
	*val = self->get_as_property_path();
	return res;
}

#ifdef __cplusplus
}
#endif
