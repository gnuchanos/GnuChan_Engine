/**************************************************************************/
/*  fps_controller.h                                                       */
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

#ifndef FPS_CONTROLLER_H
#define FPS_CONTROLLER_H

#include "scene/3d/camera.h"
#include "scene/3d/collision_shape.h"
#include "scene/3d/physics_body.h"
#include "scene/3d/ray_cast.h"
#include "scene/3d/spatial.h"

class CapsuleShape;

class FPSController : public KinematicBody {
	GDCLASS(FPSController, KinematicBody);

public:
	/* StateMachine states */
	enum MovementState {
		STATE_GROUND,
		STATE_AIR,
		STATE_CROUCH,
	};

private:
	/* MovementSystem */
	float walk_speed;
	float run_speed;
	float crouch_speed;
	float acceleration;
	float gravity;
	float jump_impulse;
	float crouch_height_scale;

	/* LookSystem */
	float mouse_sensitivity;
	bool mouse_invert_y;
	bool mouse_captured;
	float max_pitch_rad;

	/* StateMachine */
	MovementState state;
	Vector3 velocity;

	/* InteractionSystem - camera center raycast */
	float interaction_length;
	bool interaction_enabled;

	/* Node references (set via Inspector NodePaths) */
	NodePath head_path;
	NodePath camera_path;
	NodePath collision_shape_path;
	NodePath interaction_ray_path;

	Spatial *head;
	Camera *camera;
	CollisionShape *collision_shape_node;
	CapsuleShape *collision_capsule;
	RayCast *interaction_ray;

	/* Cached stance values (captured on first resolve) */
	float capsule_stand_height;
	Vector3 head_origin;
	Vector3 collision_shape_origin;
	bool stance_cached;

	void _ensure_default_children();
	void _resolve_references();
	void _apply_stance();
	void _update_state();
	void _process_movement(float p_delta);
	void _process_look(float p_delta);
	void _process_interaction();

	/* Input helpers (polled, Godot 3.x has no C++ _input virtual) */
	bool is_shift_pressed() const;
	bool is_ctrl_pressed() const;
	bool is_move_pressed(int p_key) const;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	/* MovementSystem */
	void set_walk_speed(float p_speed);
	float get_walk_speed() const;
	void set_run_speed(float p_speed);
	float get_run_speed() const;
	void set_crouch_speed(float p_speed);
	float get_crouch_speed() const;
	void set_acceleration(float p_acceleration);
	float get_acceleration() const;
	void set_gravity(float p_gravity);
	float get_gravity() const;
	void set_jump_impulse(float p_impulse);
	float get_jump_impulse() const;
	void set_crouch_height_scale(float p_scale);
	float get_crouch_height_scale() const;

	/* LookSystem */
	void set_mouse_sensitivity(float p_sensitivity);
	float get_mouse_sensitivity() const;
	void set_mouse_invert_y(bool p_invert);
	bool is_mouse_invert_y() const;
	void set_mouse_captured(bool p_captured);
	bool is_mouse_captured() const;
	void set_max_pitch_degrees(float p_degrees);
	float get_max_pitch_degrees() const;

	/* StateMachine */
	MovementState get_state() const;
	bool is_on_ground() const;
	bool is_crouching() const;

	/* InteractionSystem */
	void set_interaction_length(float p_length);
	float get_interaction_length() const;
	void set_interaction_enabled(bool p_enabled);
	bool is_interaction_enabled() const;
	bool is_interaction_colliding() const;
	Object *get_interaction_target() const;
	Vector3 get_interaction_point() const;
	Vector3 get_interaction_normal() const;

	/* Node references (code references) */
	void set_head_path(const NodePath &p_path);
	NodePath get_head_path() const;
	void set_camera_path(const NodePath &p_path);
	NodePath get_camera_path() const;
	void set_collision_shape_path(const NodePath &p_path);
	NodePath get_collision_shape_path() const;
	void set_interaction_ray_path(const NodePath &p_path);
	NodePath get_interaction_ray_path() const;

	Spatial *get_head() const;
	Camera *get_camera() const;
	CollisionShape *get_collision_shape_node() const;
	RayCast *get_interaction_ray() const;

	FPSController();
	~FPSController();
};

VARIANT_ENUM_CAST(FPSController::MovementState);

#endif // FPS_CONTROLLER_H
