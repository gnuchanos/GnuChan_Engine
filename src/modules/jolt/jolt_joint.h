#ifndef JOLT_JOINT_H
#define JOLT_JOINT_H

#include "core/rid.h"
#include "servers/physics_server.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>

class JoltJointData : public RID_Data {
public:
	PhysicsServer::JointType type;
	JPH::TwoBodyConstraint *constraint = nullptr;
	RID body_a;
	RID body_b;
	int solver_priority = 1;
	bool disable_collisions = false;

	// Pin joint data
	Vector3 pin_local_a;
	Vector3 pin_local_b;
	float pin_bias = 0.3;
	float pin_damping = 1.0;
	float pin_impulse_clamp = 0.0;

	// Hinge joint data
	Transform hinge_ref_a;
	Transform hinge_ref_b;
	Vector3 hinge_pivot_a, hinge_axis_a;
	Vector3 hinge_pivot_b, hinge_axis_b;
	float hinge_limit_upper = 3.14159f;
	float hinge_limit_lower = -3.14159f;
	float hinge_bias = 0.3;
	float hinge_limit_bias = 0.3;
	float hinge_limit_softness = 0.9;
	float hinge_limit_relaxation = 1.0;
	float hinge_motor_target_vel = 0.0;
	float hinge_motor_max_impulse = 0.0;
	bool hinge_use_limits = false;
	bool hinge_enable_motor = false;

	// Slider joint data
	Transform slider_frame_a;
	Transform slider_frame_b;
	float slider_linear_limit_upper = 0.0;
	float slider_linear_limit_lower = 0.0;
	float slider_angular_limit_upper = 0.0;
	float slider_angular_limit_lower = 0.0;

	// ConeTwist joint data
	Transform conetwist_frame_a;
	Transform conetwist_frame_b;
	float conetwist_swing_span = 3.14159f;
	float conetwist_twist_span = 3.14159f;
	float conetwist_bias = 0.3;
	float conetwist_softness = 0.5;
	float conetwist_relaxation = 1.0;

	// 6DOF joint data
	Transform dof_frame_a;
	Transform dof_frame_b;

	JoltJointData(PhysicsServer::JointType p_type) : type(p_type) {}
	~JoltJointData() override {
		// Constraints are cleaned up by PhysicsSystem::RemoveConstraint when space is freed
		constraint = nullptr;
	}
};

#endif
