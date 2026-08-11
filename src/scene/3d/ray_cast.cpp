/**************************************************************************/
/*  ray_cast.cpp                                                          */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHANIDE ENGINE                          */
/*                        https://github.com/gnuchanos                    */
/**************************************************************************/
/*  RayCast 3D:                                                          */
/*    - Query: is_colliding / get_collision_point / get_collision_normal */
/*    - Target info: body_name() / body_group() / get_collider()         */
/*    - Ignoring: skip(node) / skip_group(group)                         */
/*    - Settings: enabled / cast_to / length                             */
/*    - Editor debug visuals (used by spatial_editor_gizmos)             */
/*                                                                        */
/*  Permission is hereby granted, free of charge, to any person obtaining */
/*  a copy of this software and associated documentation files (the       */
/*  "Software"), to deal in the Software without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,   */
/*  distribute, sublicense, and/or sell copies of the Software, and to    */
/*  permit persons to whom the Software is furnished to do so, subject to */
/*  the following conditions:                                             */
/*                                                                        */
/*  The above copyright notice and this permission notice shall be        */
/*  included in all copies or substantial portions of the Software.       */
/*                                                                        */
/*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/*  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/*  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/*  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/*  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/**************************************************************************/

#include "ray_cast.h"

#include "collision_object.h"
#include "core/engine.h"
#include "mesh_instance.h"
#include "scene/main/viewport.h"
#include "servers/physics_server.h"

/* ------------------------------------------------------------------ */
/*  Query                                                              */
/* ------------------------------------------------------------------ */

bool RayCast::is_colliding() const {
	return collided;
}

Object *RayCast::get_collider() const {
	if (against == 0) {
		return nullptr;
	}
	return ObjectDB::get_instance(against);
}

Vector3 RayCast::get_collision_point() const {
	return collision_point;
}

Vector3 RayCast::get_collision_normal() const {
	return collision_normal;
}

String RayCast::get_body_name() const {
	const Object *collider = get_collider();
	if (collider == nullptr) {
		return String();
	}
	const Node *node = Object::cast_to<Node>(collider);
	if (node == nullptr) {
		return String();
	}
	return node->get_name();
}

String RayCast::get_body_group() const {
	const Object *collider = get_collider();
	if (collider == nullptr) {
		return String();
	}
	const Node *node = Object::cast_to<Node>(collider);
	if (node == nullptr) {
		return String();
	}
	List<Node::GroupInfo> groups;
	node->get_groups(&groups);
	if (groups.size() == 0) {
		return String();
	}
	return groups.front()->get().name;
}

/* ------------------------------------------------------------------ */
/*  Ignoring                                                           */
/* ------------------------------------------------------------------ */

void RayCast::skip(const Object *p_object) {
	ERR_FAIL_NULL(p_object);

	const Node *node = Object::cast_to<Node>(p_object);
	ERR_FAIL_COND_MSG(node == nullptr, "The passed object must be a Node.");

	/* Scan the node and its whole subtree, adding every physics body found
	 * to the ray's exclude set. skip(self) works even when the caller is a
	 * plain Spatial above the actual body. */
	List<const Node *> pending;
	pending.push_back(node);

	while (!pending.empty()) {
		const Node *current = pending.front()->get();
		pending.pop_front();

		const CollisionObject *co = Object::cast_to<CollisionObject>(current);
		if (co != nullptr) {
			exclude.insert(co->get_rid());
		}

		for (int i = 0; i < current->get_child_count(); i++) {
			pending.push_back(current->get_child(i));
		}
	}
}

void RayCast::skip_group(const StringName &p_group) {
	if (!is_inside_tree()) {
		return;
	}

	/* Godot 3 has no public API to list group members, so walk the whole
	 * tree from the root, find nodes in the group via is_in_group(), and
	 * add every physics body in their subtrees to the ray's exclude set. */
	List<Node *> pending;
	pending.push_back(Object::cast_to<Node>(get_tree()->get_root()));

	while (!pending.empty()) {
		Node *current = pending.front()->get();
		pending.pop_front();

		if (current->is_in_group(p_group)) {
			List<const Node *> subtree;
			subtree.push_back(current);
			while (!subtree.empty()) {
				const Node *n = subtree.front()->get();
				subtree.pop_front();

				const CollisionObject *co = Object::cast_to<CollisionObject>(n);
				if (co != nullptr) {
					exclude.insert(co->get_rid());
				}

				for (int i = 0; i < n->get_child_count(); i++) {
					subtree.push_back(n->get_child(i));
				}
			}
		}

		for (int i = 0; i < current->get_child_count(); i++) {
			pending.push_back(current->get_child(i));
		}
	}
}

/* ------------------------------------------------------------------ */
/*  Settings                                                           */
/* ------------------------------------------------------------------ */

void RayCast::set_enabled(bool p_enabled) {
	enabled = p_enabled;
	update_gizmo();

	if (is_inside_tree() && !Engine::get_singleton()->is_editor_hint()) {
		set_physics_process_internal(p_enabled);
	}
	if (!p_enabled) {
		collided = false;
	}

	if (is_inside_tree() && get_tree()->is_debugging_collisions_hint()) {
		if (p_enabled) {
			_update_debug_shape();
		} else {
			_clear_debug_shape();
		}
	}
}

bool RayCast::is_enabled() const {
	return enabled;
}

void RayCast::set_cast_to(const Vector3 &p_point) {
	cast_to = p_point;
	update_gizmo();

	if (Engine::get_singleton()->is_editor_hint()) {
		if (is_inside_tree()) {
			_update_debug_shape_vertices();
		}
	} else if (debug_shape) {
		_update_debug_shape();
	}
}

Vector3 RayCast::get_cast_to() const {
	return cast_to;
}

void RayCast::set_length(real_t p_length) {
	cast_to.z = -p_length;
	set_cast_to(cast_to);
}

real_t RayCast::get_length() const {
	return -cast_to.z;
}

void RayCast::set_collide_with_bodies(bool p_clip) {
	collide_with_bodies = p_clip;
}

bool RayCast::is_collide_with_bodies_enabled() const {
	return collide_with_bodies;
}

void RayCast::set_collide_with_areas(bool p_clip) {
	collide_with_areas = p_clip;
}

bool RayCast::is_collide_with_areas_enabled() const {
	return collide_with_areas;
}

void RayCast::force_raycast_update() {
	_update_raycast_state();
}

/* ------------------------------------------------------------------ */
/*  Internal                                                           */
/* ------------------------------------------------------------------ */

void RayCast::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (Engine::get_singleton()->is_editor_hint()) {
				_update_debug_shape_vertices();
			}
			if (enabled && !Engine::get_singleton()->is_editor_hint()) {
				set_physics_process_internal(true);
			} else {
				set_physics_process_internal(false);
			}

			if (get_tree()->is_debugging_collisions_hint()) {
				_update_debug_shape();
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (enabled) {
				set_physics_process_internal(false);
			}

			if (debug_shape) {
				_clear_debug_shape();
			}
		} break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (!enabled) {
				break;
			}

			bool prev_collision_state = collided;
			_update_raycast_state();
			if (prev_collision_state != collided && get_tree()->is_debugging_collisions_hint()) {
				_update_debug_shape_material(true);
			}
		} break;
	}
}

void RayCast::_update_raycast_state() {
	Ref<World> w3d = get_world();
	ERR_FAIL_COND(w3d.is_null());

	PhysicsDirectSpaceState *dss = PhysicsServer::get_singleton()->space_get_direct_state(w3d->get_space());
	ERR_FAIL_COND(!dss);

	Transform gt = get_global_transform();

	Vector3 to = cast_to;
	if (to == Vector3()) {
		to = Vector3(0, 0, -1);
	}

	PhysicsDirectSpaceState::RayResult rr;

	/* Collision mask is intentionally full (0xFFFFFFFF): instead of fiddling
	 * with per-object layer/mask bits, use skip() / skip_group() to ignore. */
	if (dss->intersect_ray(gt.get_origin(), gt.xform(to), rr, exclude, 0xFFFFFFFF, collide_with_bodies, collide_with_areas)) {
		collided = true;
		against = rr.collider_id;
		collision_point = rr.position;
		collision_normal = rr.normal;
	} else {
		collided = false;
		against = 0;
	}
}

/* ------------------------------------------------------------------ */
/*  Editor debug visuals (used by spatial_editor_gizmos)               */
/* ------------------------------------------------------------------ */

int RayCast::get_debug_shape_thickness() const {
	return debug_shape_thickness;
}

void RayCast::_update_debug_shape_vertices() {
	debug_shape_vertices.clear();
	debug_line_vertices.clear();

	if (cast_to == Vector3()) {
		return;
	}

	debug_line_vertices.push_back(Vector3());
	debug_line_vertices.push_back(cast_to);

	if (debug_shape_thickness > 1) {
		float scale_factor = 100.0;
		Vector3 dir = Vector3(cast_to).normalized();
		// Draw truncated pyramid
		Vector3 normal = (fabs(dir.x) + fabs(dir.y) > CMP_EPSILON) ? Vector3(-dir.y, dir.x, 0).normalized() : Vector3(0, -dir.z, dir.y).normalized();
		normal *= debug_shape_thickness / scale_factor;
		int vertices_strip_order[14] = { 4, 5, 0, 1, 2, 5, 6, 4, 7, 0, 3, 2, 7, 6 };
		for (int v = 0; v < 14; v++) {
			Vector3 vertex = vertices_strip_order[v] < 4 ? normal : normal / 3.0 + cast_to;
			debug_shape_vertices.push_back(vertex.rotated(dir, Math_PI * (0.5 * (vertices_strip_order[v] % 4) + 0.25)));
		}
	}
}

void RayCast::set_debug_shape_thickness(const int p_debug_shape_thickness) {
	debug_shape_thickness = p_debug_shape_thickness;
	update_gizmo();

	if (Engine::get_singleton()->is_editor_hint()) {
		if (is_inside_tree()) {
			_update_debug_shape_vertices();
		}
	} else if (debug_shape) {
		_update_debug_shape();
	}
}

const Vector<Vector3> &RayCast::get_debug_shape_vertices() const {
	return debug_shape_vertices;
}

const Vector<Vector3> &RayCast::get_debug_line_vertices() const {
	return debug_line_vertices;
}

void RayCast::set_debug_shape_custom_color(const Color &p_color) {
	debug_shape_custom_color = p_color;
	if (debug_material.is_valid()) {
		_update_debug_shape_material();
	}
}

Ref<Material3D> RayCast::get_debug_material() {
	_update_debug_shape_material();
	return debug_material;
}

const Color &RayCast::get_debug_shape_custom_color() const {
	return debug_shape_custom_color;
}

void RayCast::_create_debug_shape() {
	_update_debug_shape_material();

	Ref<ArrayMesh> mesh = memnew(ArrayMesh);

	MeshInstance *mi = memnew(MeshInstance);
#ifdef TOOLS_ENABLED
	// This enables the debug helper to show up in editor runs.
	// However it should not show up during export, because global mode
	// can slow the portal system, and this should only be used for debugging.
	mi->set_portal_mode(CullInstance::PORTAL_MODE_GLOBAL);
#endif
	mi->set_mesh(mesh);
	add_child(mi);

	debug_shape = mi;
}

void RayCast::_update_debug_shape_material(bool p_check_collision) {
	if (!debug_material.is_valid()) {
		Ref<SpatialMaterial> material = memnew(SpatialMaterial);
		debug_material = material;

		material->set_flag(Material3D::FLAG_UNSHADED, true);
		material->set_feature(Material3D::FEATURE_TRANSPARENT, true);
		// Use double-sided rendering so that the RayCast can be seen if the camera is inside.
		material->set_cull_mode(Material3D::CULL_DISABLED);
	}

	Color color = debug_shape_custom_color;
	if (color == Color(0.0, 0.0, 0.0)) {
		// Use the default debug shape color defined in the Project Settings.
		color = get_tree()->get_debug_collisions_color();
	}

	if (p_check_collision && collided) {
		if ((color.get_h() < 0.055 || color.get_h() > 0.945) && color.get_s() > 0.5 && color.get_v() > 0.5) {
			// If base color is already quite reddish, highlight collision with green color
			color = Color(0.0, 1.0, 0.0, color.a);
		} else {
			// Else, highlight collision with red color
			color = Color(1.0, 0, 0, color.a);
		}
	}

	Ref<Material3D> material = static_cast<Ref<Material3D>>(debug_material);
	material->set_albedo(color);
}

void RayCast::_update_debug_shape() {
	if (!enabled) {
		return;
	}

	if (!debug_shape) {
		_create_debug_shape();
	}

	MeshInstance *mi = static_cast<MeshInstance *>(debug_shape);
	Ref<ArrayMesh> mesh = mi->get_mesh();
	if (!mesh.is_valid()) {
		return;
	}

	_update_debug_shape_vertices();

	mesh->clear_surfaces();

	Array a;
	a.resize(Mesh::ARRAY_MAX);

	uint32_t flags = 0;
	int surface_count = 0;

	if (!debug_line_vertices.empty()) {
		a[Mesh::ARRAY_VERTEX] = debug_line_vertices;
		mesh->add_surface_from_arrays(Mesh::PRIMITIVE_LINES, a, Array(), flags);
		mesh->surface_set_material(surface_count, debug_material);
		++surface_count;
	}

	if (!debug_shape_vertices.empty()) {
		a[Mesh::ARRAY_VERTEX] = debug_shape_vertices;
		mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLE_STRIP, a, Array(), flags);
		mesh->surface_set_material(surface_count, debug_material);
		++surface_count;
	}
}

void RayCast::_clear_debug_shape() {
	if (!debug_shape) {
		return;
	}

	MeshInstance *mi = static_cast<MeshInstance *>(debug_shape);
	if (mi->is_inside_tree()) {
		mi->queue_delete();
	} else {
		memdelete(mi);
	}

	debug_shape = nullptr;
}

void RayCast::_bind_methods() {
	/* Query */
	ClassDB::bind_method(D_METHOD("is_colliding"), &RayCast::is_colliding);
	ClassDB::bind_method(D_METHOD("get_collider"), &RayCast::get_collider);
	ClassDB::bind_method(D_METHOD("get_collision_point"), &RayCast::get_collision_point);
	ClassDB::bind_method(D_METHOD("get_collision_normal"), &RayCast::get_collision_normal);

	ClassDB::bind_method(D_METHOD("body_name"), &RayCast::get_body_name);
	ClassDB::bind_method(D_METHOD("body_group"), &RayCast::get_body_group);

	/* Ignoring */
	ClassDB::bind_method(D_METHOD("skip", "node"), &RayCast::skip);
	ClassDB::bind_method(D_METHOD("skip_group", "group"), &RayCast::skip_group);

	/* Settings */
	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &RayCast::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &RayCast::is_enabled);

	ClassDB::bind_method(D_METHOD("set_cast_to", "local_point"), &RayCast::set_cast_to);
	ClassDB::bind_method(D_METHOD("get_cast_to"), &RayCast::get_cast_to);

	ClassDB::bind_method(D_METHOD("set_length", "length"), &RayCast::set_length);
	ClassDB::bind_method(D_METHOD("get_length"), &RayCast::get_length);

	ClassDB::bind_method(D_METHOD("set_collide_with_areas", "enable"), &RayCast::set_collide_with_areas);
	ClassDB::bind_method(D_METHOD("is_collide_with_areas_enabled"), &RayCast::is_collide_with_areas_enabled);

	ClassDB::bind_method(D_METHOD("set_collide_with_bodies", "enable"), &RayCast::set_collide_with_bodies);
	ClassDB::bind_method(D_METHOD("is_collide_with_bodies_enabled"), &RayCast::is_collide_with_bodies_enabled);

	ClassDB::bind_method(D_METHOD("force_raycast_update"), &RayCast::force_raycast_update);

	/* Editor debug visuals */
	ClassDB::bind_method(D_METHOD("set_debug_shape_custom_color", "debug_shape_custom_color"), &RayCast::set_debug_shape_custom_color);
	ClassDB::bind_method(D_METHOD("get_debug_shape_custom_color"), &RayCast::get_debug_shape_custom_color);
	ClassDB::bind_method(D_METHOD("set_debug_shape_thickness", "debug_shape_thickness"), &RayCast::set_debug_shape_thickness);
	ClassDB::bind_method(D_METHOD("get_debug_shape_thickness"), &RayCast::get_debug_shape_thickness);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "cast_to"), "set_cast_to", "get_cast_to");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "length"), "set_length", "get_length");

	ADD_GROUP("Collide With", "collide_with");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_areas"), "set_collide_with_areas", "is_collide_with_areas_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_bodies"), "set_collide_with_bodies", "is_collide_with_bodies_enabled");

	ADD_GROUP("Debug Shape", "debug_shape");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "debug_shape_custom_color"), "set_debug_shape_custom_color", "get_debug_shape_custom_color");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "debug_shape_thickness", PROPERTY_HINT_RANGE, "1,5"), "set_debug_shape_thickness", "get_debug_shape_thickness");
}

RayCast::RayCast() {
	enabled = false;
	against = 0;
	collided = false;
	cast_to = Vector3(0, 0, -3);
	debug_shape = nullptr;
	collide_with_bodies = true;
	collide_with_areas = false;
}
