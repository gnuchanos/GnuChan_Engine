#include "jolt_body.h"

Vector3 JoltPhysicsDirectBodyState::get_total_gravity() const { return Vector3(0, -9.81, 0); }
float JoltPhysicsDirectBodyState::get_total_angular_damp() const { return 0.0; }
float JoltPhysicsDirectBodyState::get_total_linear_damp() const { return 0.0; }
Vector3 JoltPhysicsDirectBodyState::get_center_of_mass() const { return Vector3(); }
Basis JoltPhysicsDirectBodyState::get_principal_inertia_axes() const { return Basis(); }
float JoltPhysicsDirectBodyState::get_inverse_mass() const { return 1.0; }
Vector3 JoltPhysicsDirectBodyState::get_inverse_inertia() const { return Vector3(1, 1, 1); }
Basis JoltPhysicsDirectBodyState::get_inverse_inertia_tensor() const { return Basis(); }
void JoltPhysicsDirectBodyState::set_linear_velocity(const Vector3 &p_velocity) {
	if (body_data) body_data->linear_velocity = p_velocity;
}
Vector3 JoltPhysicsDirectBodyState::get_linear_velocity() const {
	return body_data ? body_data->linear_velocity : Vector3();
}
void JoltPhysicsDirectBodyState::set_angular_velocity(const Vector3 &p_velocity) {
	if (body_data) body_data->angular_velocity = p_velocity;
}
Vector3 JoltPhysicsDirectBodyState::get_angular_velocity() const {
	return body_data ? body_data->angular_velocity : Vector3();
}
void JoltPhysicsDirectBodyState::set_transform(const Transform &p_transform) {
	if (body_data) body_data->transform = p_transform;
}
Transform JoltPhysicsDirectBodyState::get_transform() const {
	return body_data ? body_data->transform : Transform();
}
Vector3 JoltPhysicsDirectBodyState::get_velocity_at_local_position(const Vector3 &p_position) const {
	return get_linear_velocity() + get_angular_velocity().cross(p_position);
}
void JoltPhysicsDirectBodyState::add_central_force(const Vector3 &p_force) {}
void JoltPhysicsDirectBodyState::add_force(const Vector3 &p_force, const Vector3 &p_pos) {}
void JoltPhysicsDirectBodyState::add_torque(const Vector3 &p_torque) {}
void JoltPhysicsDirectBodyState::apply_central_impulse(const Vector3 &p_j) {}
void JoltPhysicsDirectBodyState::apply_impulse(const Vector3 &p_pos, const Vector3 &p_j) {}
void JoltPhysicsDirectBodyState::apply_torque_impulse(const Vector3 &p_j) {}
void JoltPhysicsDirectBodyState::set_sleep_state(bool p_enable) {}
bool JoltPhysicsDirectBodyState::is_sleeping() const { return false; }
int JoltPhysicsDirectBodyState::get_contact_count() const { return 0; }
Vector3 JoltPhysicsDirectBodyState::get_contact_local_position(int p_contact_idx) const { return Vector3(); }
Vector3 JoltPhysicsDirectBodyState::get_contact_local_normal(int p_contact_idx) const { return Vector3(); }
float JoltPhysicsDirectBodyState::get_contact_impulse(int p_contact_idx) const { return 0.0; }
int JoltPhysicsDirectBodyState::get_contact_local_shape(int p_contact_idx) const { return 0; }
RID JoltPhysicsDirectBodyState::get_contact_collider(int p_contact_idx) const { return RID(); }
Vector3 JoltPhysicsDirectBodyState::get_contact_collider_position(int p_contact_idx) const { return Vector3(); }
ObjectID JoltPhysicsDirectBodyState::get_contact_collider_id(int p_contact_idx) const { return ObjectID(0); }
int JoltPhysicsDirectBodyState::get_contact_collider_shape(int p_contact_idx) const { return 0; }
Vector3 JoltPhysicsDirectBodyState::get_contact_collider_velocity_at_position(int p_contact_idx) const { return Vector3(); }
real_t JoltPhysicsDirectBodyState::get_step() const { return 1.0 / 60.0; }
PhysicsDirectSpaceState *JoltPhysicsDirectBodyState::get_space_state() { return nullptr; }
