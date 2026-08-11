/**************************************************************************/
/*  fps_controller.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHANIDE ENGINE                          */
/*                        https://github.com/gnuchanos                    */
/**************************************************************************/
/*  FPSController: built-in first person controller node.                 */
/*  Based on the design in fps_node.md.                                   */
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

#include "fps_controller.h"

#include "core/engine.h"
#include "core/math/math_funcs.h"
#include "core/os/input.h"
#include "core/os/keyboard.h"
#include "core/print_string.h"
#include "scene/resources/capsule_shape.h"
#include "servers/physics_server.h"

/* ------------------------------------------------------------------ */
/*  Construction                                                       */
/* ------------------------------------------------------------------ */

FPSController::FPSController() :
		walk_speed(5.0),
		run_speed(8.0),
		crouch_speed(2.5),
		acceleration(10.0),
		gravity(25.0),
		jump_impulse(9.0),
		crouch_height_scale(0.6),
		mouse_sensitivity(0.002),
		mouse_invert_y(false),
		mouse_captured(true),
		max_pitch_rad(Math::deg2rad(90.0)),
		state(STATE_GROUND),
		interaction_length(3.0),
		interaction_enabled(true),
		head(nullptr),
		camera(nullptr),
		collision_shape_node(nullptr),
		collision_capsule(nullptr),
		interaction_ray(nullptr),
		capsule_stand_height(1.8),
		stance_cached(false) {}

FPSController::~FPSController() {}

/* ------------------------------------------------------------------ */
/*  Notification                                                       */
/* ------------------------------------------------------------------ */

void FPSController::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			if (!Engine::get_singleton()->is_editor_hint()) {
				set_physics_process(true);
			}
			break;
		case NOTIFICATION_EXIT_TREE:
			if (!Engine::get_singleton()->is_editor_hint()) {
				set_physics_process(false);
			}
			break;
		case NOTIFICATION_READY:
			_ensure_default_children();
			_resolve_references();
			_apply_stance();
			if (mouse_captured && !Engine::get_singleton()->is_editor_hint()) {
				Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
			}
			break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			float delta = get_physics_process_delta_time();
			_update_state();
			_process_movement(delta);
			_process_look(delta);
			_process_interaction();
		} break;
	}
}

/* ------------------------------------------------------------------ */
/*  Binding                                                            */
/* ------------------------------------------------------------------ */

void FPSController::_bind_methods() {
	/* MovementSystem */
	ClassDB::bind_method(D_METHOD("set_walk_speed", "speed"), &FPSController::set_walk_speed);
	ClassDB::bind_method(D_METHOD("get_walk_speed"), &FPSController::get_walk_speed);
	ClassDB::bind_method(D_METHOD("set_run_speed", "speed"), &FPSController::set_run_speed);
	ClassDB::bind_method(D_METHOD("get_run_speed"), &FPSController::get_run_speed);
	ClassDB::bind_method(D_METHOD("set_crouch_speed", "speed"), &FPSController::set_crouch_speed);
	ClassDB::bind_method(D_METHOD("get_crouch_speed"), &FPSController::get_crouch_speed);
	ClassDB::bind_method(D_METHOD("set_acceleration", "acceleration"), &FPSController::set_acceleration);
	ClassDB::bind_method(D_METHOD("get_acceleration"), &FPSController::get_acceleration);
	ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &FPSController::set_gravity);
	ClassDB::bind_method(D_METHOD("get_gravity"), &FPSController::get_gravity);
	ClassDB::bind_method(D_METHOD("set_jump_impulse", "impulse"), &FPSController::set_jump_impulse);
	ClassDB::bind_method(D_METHOD("get_jump_impulse"), &FPSController::get_jump_impulse);
	ClassDB::bind_method(D_METHOD("set_crouch_height_scale", "scale"), &FPSController::set_crouch_height_scale);
	ClassDB::bind_method(D_METHOD("get_crouch_height_scale"), &FPSController::get_crouch_height_scale);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "walk_speed"), "set_walk_speed", "get_walk_speed");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "run_speed"), "set_run_speed", "get_run_speed");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "crouch_speed"), "set_crouch_speed", "get_crouch_speed");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "acceleration"), "set_acceleration", "get_acceleration");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "gravity"), "set_gravity", "get_gravity");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "jump_impulse"), "set_jump_impulse", "get_jump_impulse");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "crouch_height_scale"), "set_crouch_height_scale", "get_crouch_height_scale");

	/* LookSystem */
	ClassDB::bind_method(D_METHOD("set_mouse_sensitivity", "sensitivity"), &FPSController::set_mouse_sensitivity);
	ClassDB::bind_method(D_METHOD("get_mouse_sensitivity"), &FPSController::get_mouse_sensitivity);
	ClassDB::bind_method(D_METHOD("set_mouse_invert_y", "invert"), &FPSController::set_mouse_invert_y);
	ClassDB::bind_method(D_METHOD("is_mouse_invert_y"), &FPSController::is_mouse_invert_y);
	ClassDB::bind_method(D_METHOD("set_mouse_captured", "captured"), &FPSController::set_mouse_captured);
	ClassDB::bind_method(D_METHOD("is_mouse_captured"), &FPSController::is_mouse_captured);
	ClassDB::bind_method(D_METHOD("set_max_pitch_degrees", "degrees"), &FPSController::set_max_pitch_degrees);
	ClassDB::bind_method(D_METHOD("get_max_pitch_degrees"), &FPSController::get_max_pitch_degrees);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "mouse_sensitivity"), "set_mouse_sensitivity", "get_mouse_sensitivity");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "mouse_invert_y"), "set_mouse_invert_y", "is_mouse_invert_y");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "mouse_captured"), "set_mouse_captured", "is_mouse_captured");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_pitch_degrees"), "set_max_pitch_degrees", "get_max_pitch_degrees");

	/* StateMachine */
	ClassDB::bind_method(D_METHOD("get_state"), &FPSController::get_state);
	ClassDB::bind_method(D_METHOD("is_crouching"), &FPSController::is_crouching);

	/* InteractionSystem */
	ClassDB::bind_method(D_METHOD("set_interaction_length", "length"), &FPSController::set_interaction_length);
	ClassDB::bind_method(D_METHOD("get_interaction_length"), &FPSController::get_interaction_length);
	ClassDB::bind_method(D_METHOD("set_interaction_enabled", "enabled"), &FPSController::set_interaction_enabled);
	ClassDB::bind_method(D_METHOD("is_interaction_enabled"), &FPSController::is_interaction_enabled);
	ClassDB::bind_method(D_METHOD("is_interaction_colliding"), &FPSController::is_interaction_colliding);
	ClassDB::bind_method(D_METHOD("get_interaction_target"), &FPSController::get_interaction_target);
	ClassDB::bind_method(D_METHOD("get_interaction_point"), &FPSController::get_interaction_point);
	ClassDB::bind_method(D_METHOD("get_interaction_normal"), &FPSController::get_interaction_normal);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "interaction_length"), "set_interaction_length", "get_interaction_length");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "interaction_enabled"), "set_interaction_enabled", "is_interaction_enabled");

	/* Node references (code references, set in Inspector) */
	ClassDB::bind_method(D_METHOD("set_head_path", "path"), &FPSController::set_head_path);
	ClassDB::bind_method(D_METHOD("get_head_path"), &FPSController::get_head_path);
	ClassDB::bind_method(D_METHOD("set_camera_path", "path"), &FPSController::set_camera_path);
	ClassDB::bind_method(D_METHOD("get_camera_path"), &FPSController::get_camera_path);
	ClassDB::bind_method(D_METHOD("set_collision_shape_path", "path"), &FPSController::set_collision_shape_path);
	ClassDB::bind_method(D_METHOD("get_collision_shape_path"), &FPSController::get_collision_shape_path);
	ClassDB::bind_method(D_METHOD("set_interaction_ray_path", "path"), &FPSController::set_interaction_ray_path);
	ClassDB::bind_method(D_METHOD("get_interaction_ray_path"), &FPSController::get_interaction_ray_path);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "head_path"), "set_head_path", "get_head_path");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "camera_path"), "set_camera_path", "get_camera_path");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "collision_shape_path"), "set_collision_shape_path", "get_collision_shape_path");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "interaction_ray_path"), "set_interaction_ray_path", "get_interaction_ray_path");

	ClassDB::bind_method(D_METHOD("get_head"), &FPSController::get_head);
	ClassDB::bind_method(D_METHOD("get_camera"), &FPSController::get_camera);
	ClassDB::bind_method(D_METHOD("get_collision_shape_node"), &FPSController::get_collision_shape_node);
	ClassDB::bind_method(D_METHOD("get_interaction_ray"), &FPSController::get_interaction_ray);

	BIND_ENUM_CONSTANT(STATE_GROUND);
	BIND_ENUM_CONSTANT(STATE_AIR);
	BIND_ENUM_CONSTANT(STATE_CROUCH);
}

/* ------------------------------------------------------------------ */
/*  Default child hierarchy (idempotent)                               */
/*  Finds existing children by name first, creates them only if        */
/*  missing, and always fills the Inspector NodePaths so the node      */
/*  works out of the box without manual reference wiring.              */
/* ------------------------------------------------------------------ */

void FPSController::_ensure_default_children() {
	if (!is_inside_tree()) {
		return;
	}

	Node *head_node = has_node(NodePath("Head")) ? get_node(NodePath("Head")) : nullptr;
	head = Object::cast_to<Spatial>(head_node);
	if (head == nullptr) {
		head = memnew(Spatial);
		head->set_name("Head");
		head->set_translation(Vector3(0, 2.138, 0));
		add_child(head);
		head->set_owner(this);
	}
	head_path = head->get_path_to(this) == NodePath() ? head->get_path() : get_path_to(head);

	Node *camera_node = has_node(NodePath("Head/Camera")) ? get_node(NodePath("Head/Camera")) : nullptr;
	camera = Object::cast_to<Camera>(camera_node);
	if (camera == nullptr) {
		camera = memnew(Camera);
		camera->set_name("Camera");
		camera->set_fov(75.0);
		head->add_child(camera);
		camera->set_owner(this);
	}
	camera_path = get_path_to(camera);

	Node *shape_node = has_node(NodePath("CollisionShape")) ? get_node(NodePath("CollisionShape")) : nullptr;
	collision_shape_node = Object::cast_to<CollisionShape>(shape_node);
	if (collision_shape_node == nullptr) {
		Ref<CapsuleShape> capsule = memnew(CapsuleShape);
		capsule->set_radius(0.4);
		capsule->set_height(1.8);

		collision_shape_node = memnew(CollisionShape);
		collision_shape_node->set_name("CollisionShape");
		collision_shape_node->set_shape(capsule);
		collision_shape_node->set_translation(Vector3(0, 0.9, 0));
		collision_shape_node->rotate_x(Math_PI / 2.0); // capsule long axis is Z in Godot 3.x; stand it upright
		add_child(collision_shape_node);
		collision_shape_node->set_owner(this);
	}
	collision_shape_path = get_path_to(collision_shape_node);

	Node *ray_node = has_node(NodePath("Head/InteractionRay")) ? get_node(NodePath("Head/InteractionRay")) : nullptr;
	interaction_ray = Object::cast_to<RayCast>(ray_node);
	if (interaction_ray == nullptr) {
		interaction_ray = memnew(RayCast);
		interaction_ray->set_name("InteractionRay");
		interaction_ray->set_cast_to(Vector3(0, 0, -interaction_length));
		head->add_child(interaction_ray);
		interaction_ray->set_owner(this);
	}
	/* Never let the ray hit the player's own body: the RayCast parent is Head
	 * (Spatial), so its built-in exclude_parent_body cannot see this KinematicBody.
	 * skip(this) walks this node's subtree and excludes every physics body,
	 * so the player capsule is always ignored. Scripts can add more ignores
	 * at any time via $Head/InteractionRay.skip(node). */
	interaction_ray->skip(this);
	interaction_ray_path = get_path_to(interaction_ray);
}

/* ------------------------------------------------------------------ */
/*  Reference resolution                                               */
/* ------------------------------------------------------------------ */

void FPSController::_resolve_references() {
	head = Object::cast_to<Spatial>(get_node_or_null(head_path));
	camera = Object::cast_to<Camera>(get_node_or_null(camera_path));
	collision_shape_node = Object::cast_to<CollisionShape>(get_node_or_null(collision_shape_path));
	interaction_ray = Object::cast_to<RayCast>(get_node_or_null(interaction_ray_path));

	collision_capsule = nullptr;
	if (collision_shape_node != nullptr) {
		Ref<Shape> shape = collision_shape_node->get_shape();
		if (shape.is_valid()) {
			collision_capsule = Object::cast_to<CapsuleShape>(shape.ptr());
		}
	}
}

void FPSController::_apply_stance() {
	if (head == nullptr || collision_shape_node == nullptr || collision_capsule == nullptr) {
		return;
	}

	if (!stance_cached) {
		capsule_stand_height = collision_capsule->get_height();
		head_origin = head->get_translation();
		collision_shape_origin = collision_shape_node->get_translation();
		stance_cached = true;
	}

	float scale = is_crouching() ? crouch_height_scale : 1.0;

	collision_capsule->set_height(capsule_stand_height * scale);

	Vector3 shape_pos = collision_shape_origin;
	shape_pos.y = capsule_stand_height * scale * 0.5;
	collision_shape_node->set_translation(shape_pos);

	Vector3 eye = head_origin;
	eye.y = head_origin.y * scale;
	head->set_translation(eye);
}

/* ------------------------------------------------------------------ */
/*  MovementSystem                                                     */
/* ------------------------------------------------------------------ */

void FPSController::_update_state() {
	if (is_on_floor()) {
		if (is_ctrl_pressed()) {
			state = STATE_CROUCH;
		} else {
			state = STATE_GROUND;
		}
	} else {
		state = STATE_AIR;
	}
}

bool FPSController::is_shift_pressed() const {
	return Input::get_singleton()->is_physical_key_pressed(KEY_SHIFT);
}

bool FPSController::is_ctrl_pressed() const {
	return Input::get_singleton()->is_physical_key_pressed(KEY_CONTROL);
}

bool FPSController::is_move_pressed(int p_key) const {
	return Input::get_singleton()->is_physical_key_pressed(p_key);
}

void FPSController::_process_movement(float p_delta) {
	/* WASD input. -Z is forward, so W is +1 and S is -1. */
	Vector3 input_dir;
	if (is_move_pressed(KEY_W)) {
		input_dir.z += 1.0; // forward
	}
	if (is_move_pressed(KEY_S)) {
		input_dir.z -= 1.0; // backward
	}
	if (is_move_pressed(KEY_A)) {
		input_dir.x -= 1.0; // left
	}
	if (is_move_pressed(KEY_D)) {
		input_dir.x += 1.0; // right
	}

	if (input_dir.length_squared() > 0.0) {
		input_dir = input_dir.normalized();
	}

	/* Target speed: crouch < walk < run */
	float target_speed = walk_speed;
	if (is_crouching()) {
		target_speed = crouch_speed;
	} else if (is_shift_pressed()) {
		target_speed = run_speed;
	}

	/* Align wish direction with body yaw */
	Transform body_xform = get_global_transform();
	Vector3 forward = -body_xform.basis.get_axis(2);
	Vector3 right = body_xform.basis.get_axis(0);
	Vector3 wish_dir = forward * input_dir.z + right * input_dir.x;
	if (wish_dir.length_squared() > 0.0) {
		wish_dir = wish_dir.normalized();
	}

	Vector3 horizontal_velocity = wish_dir * target_speed;
	velocity.x = Math::lerp(velocity.x, horizontal_velocity.x, acceleration * p_delta);
	velocity.z = Math::lerp(velocity.z, horizontal_velocity.z, acceleration * p_delta);

	/* Gravity + jump */
	if (is_on_floor()) {
		if (!is_crouching() && is_move_pressed(KEY_SPACE)) {
			velocity.y = jump_impulse;
		} else {
			velocity.y = 0.0;
		}
	} else {
		velocity.y -= gravity * p_delta;
	}

	velocity = move_and_slide(velocity, Vector3(0, 1, 0));
}

/* ------------------------------------------------------------------ */
/*  LookSystem                                                         */
/* ------------------------------------------------------------------ */

void FPSController::_process_look(float p_delta) {
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	_apply_stance();

	if (!mouse_captured || head == nullptr) {
		return;
	}

	Input *input = Input::get_singleton();
	if (input == nullptr) {
		return;
	}

	/* The Windows backend uses raw input in captured mode: it feeds the real
	 * pixel delta through InputEventMouseMotion.relative, which updates the
	 * SpeedTrack backing Input::get_last_mouse_speed(). Center-based polling
	 * (get_mouse_position) never changes in captured mode, so this is the only
	 * correct delta source for a C++ node in Godot 3.x. */
	Vector2 relative = input->get_last_mouse_speed() * p_delta;

	if (relative.length_squared() > 0.0) {
		float yaw = -relative.x * mouse_sensitivity;
		float pitch = -relative.y * mouse_sensitivity;
		if (mouse_invert_y) {
			pitch = -pitch;
		}

		rotate_y(yaw);
		head->rotate_x(pitch);

		/* Clamp pitch to avoid camera flipping */
		float clamped_pitch = CLAMP(head->get_rotation().x, -max_pitch_rad, max_pitch_rad);
		head->set_rotation(Vector3(clamped_pitch, 0, 0));
	}
}

/* ------------------------------------------------------------------ */
/*  InteractionSystem                                                  */
/* ------------------------------------------------------------------ */

void FPSController::_process_interaction() {
	if (interaction_ray == nullptr) {
		return;
	}

	/* The RayCast node's own cast_to is authoritative (it can be edited on the
	 * node in the editor). We must NOT overwrite it every frame, otherwise any
	 * manual length change on the node is ignored at runtime. The
	 * interaction_length property propagates through set_interaction_length()
	 * instead, so both ways to set the length keep working. */
	interaction_ray->set_enabled(interaction_enabled);
}

/* ------------------------------------------------------------------ */
/*  MovementSystem getters/setters                                     */
/* ------------------------------------------------------------------ */

void FPSController::set_walk_speed(float p_speed) {
	walk_speed = p_speed;
}
float FPSController::get_walk_speed() const {
	return walk_speed;
}

void FPSController::set_run_speed(float p_speed) {
	run_speed = p_speed;
}
float FPSController::get_run_speed() const {
	return run_speed;
}

void FPSController::set_crouch_speed(float p_speed) {
	crouch_speed = p_speed;
}
float FPSController::get_crouch_speed() const {
	return crouch_speed;
}

void FPSController::set_acceleration(float p_acceleration) {
	acceleration = p_acceleration;
}
float FPSController::get_acceleration() const {
	return acceleration;
}

void FPSController::set_gravity(float p_gravity) {
	gravity = p_gravity;
}
float FPSController::get_gravity() const {
	return gravity;
}

void FPSController::set_jump_impulse(float p_impulse) {
	jump_impulse = p_impulse;
}
float FPSController::get_jump_impulse() const {
	return jump_impulse;
}

void FPSController::set_crouch_height_scale(float p_scale) {
	crouch_height_scale = p_scale;
}
float FPSController::get_crouch_height_scale() const {
	return crouch_height_scale;
}

/* ------------------------------------------------------------------ */
/*  LookSystem getters/setters                                         */
/* ------------------------------------------------------------------ */

void FPSController::set_mouse_sensitivity(float p_sensitivity) {
	mouse_sensitivity = p_sensitivity;
}
float FPSController::get_mouse_sensitivity() const {
	return mouse_sensitivity;
}

void FPSController::set_mouse_invert_y(bool p_invert) {
	mouse_invert_y = p_invert;
}
bool FPSController::is_mouse_invert_y() const {
	return mouse_invert_y;
}

void FPSController::set_mouse_captured(bool p_captured) {
	mouse_captured = p_captured;
	if (is_inside_tree() && !Engine::get_singleton()->is_editor_hint()) {
		Input::get_singleton()->set_mouse_mode(p_captured ? Input::MOUSE_MODE_CAPTURED : Input::MOUSE_MODE_VISIBLE);
	}
}
bool FPSController::is_mouse_captured() const {
	return mouse_captured;
}

void FPSController::set_max_pitch_degrees(float p_degrees) {
	max_pitch_rad = Math::deg2rad(p_degrees);
}
float FPSController::get_max_pitch_degrees() const {
	return Math::rad2deg(max_pitch_rad);
}

/* ------------------------------------------------------------------ */
/*  StateMachine                                                       */
/* ------------------------------------------------------------------ */

FPSController::MovementState FPSController::get_state() const {
	return state;
}

bool FPSController::is_on_ground() const {
	return is_on_floor();
}

bool FPSController::is_crouching() const {
	return state == STATE_CROUCH;
}

/* ------------------------------------------------------------------ */
/*  InteractionSystem getters/setters                                  */
/* ------------------------------------------------------------------ */

void FPSController::set_interaction_length(float p_length) {
	interaction_length = p_length;
	/* Keep the RayCast node in sync when set from the FPSController inspector. */
	if (interaction_ray != nullptr) {
		interaction_ray->set_cast_to(Vector3(0, 0, -p_length));
	}
}
float FPSController::get_interaction_length() const {
	/* The RayCast node's own cast_to is authoritative: editing it on the node
	 * (or scaling it in a script) is reflected here automatically. */
	if (interaction_ray != nullptr) {
		return -interaction_ray->get_cast_to().z;
	}
	return interaction_length;
}

void FPSController::set_interaction_enabled(bool p_enabled) {
	interaction_enabled = p_enabled;
}
bool FPSController::is_interaction_enabled() const {
	return interaction_enabled;
}

bool FPSController::is_interaction_colliding() const {
	return interaction_ray != nullptr && interaction_ray->is_colliding();
}

Object *FPSController::get_interaction_target() const {
	return interaction_ray != nullptr ? interaction_ray->get_collider() : nullptr;
}

Vector3 FPSController::get_interaction_point() const {
	return interaction_ray != nullptr ? interaction_ray->get_collision_point() : Vector3();
}

Vector3 FPSController::get_interaction_normal() const {
	return interaction_ray != nullptr ? interaction_ray->get_collision_normal() : Vector3();
}

/* ------------------------------------------------------------------ */
/*  Node references (code references)                                  */
/* ------------------------------------------------------------------ */

void FPSController::set_head_path(const NodePath &p_path) {
	head_path = p_path;
}
NodePath FPSController::get_head_path() const {
	return head_path;
}

void FPSController::set_camera_path(const NodePath &p_path) {
	camera_path = p_path;
}
NodePath FPSController::get_camera_path() const {
	return camera_path;
}

void FPSController::set_collision_shape_path(const NodePath &p_path) {
	collision_shape_path = p_path;
}
NodePath FPSController::get_collision_shape_path() const {
	return collision_shape_path;
}

void FPSController::set_interaction_ray_path(const NodePath &p_path) {
	interaction_ray_path = p_path;
}
NodePath FPSController::get_interaction_ray_path() const {
	return interaction_ray_path;
}

Spatial *FPSController::get_head() const {
	return head;
}

Camera *FPSController::get_camera() const {
	return camera;
}

CollisionShape *FPSController::get_collision_shape_node() const {
	return collision_shape_node;
}

RayCast *FPSController::get_interaction_ray() const {
	return interaction_ray;
}
