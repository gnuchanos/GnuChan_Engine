#ifndef JOLT_BODY_H
#define JOLT_BODY_H

#include "core/rid.h"
#include "servers/physics_server.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

class JoltBodyData : public RID_Data {
public:
	JPH::BodyID body_id;
	RID space_rid;
	ObjectID instance_id = 0;
	PhysicsServer::BodyMode mode = PhysicsServer::BODY_MODE_STATIC;
	uint32_t collision_layer = 1;
	uint32_t collision_mask = 1;
	uint32_t user_flags = 0;
	bool init_sleeping = false;
	bool ccd_enabled = false;
	bool omit_force_integration = false;
	bool ray_pickable = true;
	Vector<RID> shapes;
	Vector<Transform> shape_transforms;
	Vector<bool> shape_disabled;
	uint32_t axis_locks = 0;
	Set<RID> collision_exceptions;
	real_t bounce = 0.0;
	real_t friction = 1.0;
	real_t mass = 1.0;
	real_t gravity_scale = 1.0;
	real_t linear_damp = 0.0;
	real_t angular_damp = 0.0;
	real_t kinematic_safe_margin = 0.001;
	int max_contacts_reported = 0;
	float contacts_depth_threshold = 0.0;
	Object *force_integration_cb_receiver = nullptr;
	StringName force_integration_cb_method;
	Variant force_integration_cb_udata;
	class JoltPhysicsDirectBodyState *direct_state = nullptr;

	// Fields used by JoltPhysicsDirectBodyState
	Vector3 linear_velocity;
	Vector3 angular_velocity;
	Transform transform;

	JoltBodyData() = default;
	~JoltBodyData() override = default;
};

class JoltPhysicsDirectBodyState : public PhysicsDirectBodyState {
	GDCLASS(JoltPhysicsDirectBodyState, PhysicsDirectBodyState);
public:
	JoltBodyData *body_data = nullptr;
	virtual Vector3 get_total_gravity() const override;
	virtual float get_total_angular_damp() const override;
	virtual float get_total_linear_damp() const override;
	virtual Vector3 get_center_of_mass() const override;
	virtual Basis get_principal_inertia_axes() const override;
	virtual float get_inverse_mass() const override;
	virtual Vector3 get_inverse_inertia() const override;
	virtual Basis get_inverse_inertia_tensor() const override;
	virtual void set_linear_velocity(const Vector3 &p_velocity) override;
	virtual Vector3 get_linear_velocity() const override;
	virtual void set_angular_velocity(const Vector3 &p_velocity) override;
	virtual Vector3 get_angular_velocity() const override;
	virtual void set_transform(const Transform &p_transform) override;
	virtual Transform get_transform() const override;
	virtual Vector3 get_velocity_at_local_position(const Vector3 &p_position) const override;
	virtual void add_central_force(const Vector3 &p_force) override;
	virtual void add_force(const Vector3 &p_force, const Vector3 &p_pos) override;
	virtual void add_torque(const Vector3 &p_torque) override;
	virtual void apply_central_impulse(const Vector3 &p_j) override;
	virtual void apply_impulse(const Vector3 &p_pos, const Vector3 &p_j) override;
	virtual void apply_torque_impulse(const Vector3 &p_j) override;
	virtual void set_sleep_state(bool p_enable) override;
	virtual bool is_sleeping() const override;
	virtual int get_contact_count() const override;
	virtual Vector3 get_contact_local_position(int p_contact_idx) const override;
	virtual Vector3 get_contact_local_normal(int p_contact_idx) const override;
	virtual float get_contact_impulse(int p_contact_idx) const override;
	virtual int get_contact_local_shape(int p_contact_idx) const override;
	virtual RID get_contact_collider(int p_contact_idx) const override;
	virtual Vector3 get_contact_collider_position(int p_contact_idx) const override;
	virtual ObjectID get_contact_collider_id(int p_contact_idx) const override;
	virtual int get_contact_collider_shape(int p_contact_idx) const override;
	virtual Vector3 get_contact_collider_velocity_at_position(int p_contact_idx) const override;
	virtual real_t get_step() const override;
	virtual PhysicsDirectSpaceState *get_space_state() override;
};

#endif
