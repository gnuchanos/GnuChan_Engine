#ifndef JOLT_AREA_H
#define JOLT_AREA_H

#include "core/rid.h"
#include "core/object.h"
#include "servers/physics_server.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

class JoltAreaData : public RID_Data {
public:
	JPH::BodyID body_id;
	RID space_rid;
	RID space;
	ObjectID instance_id = 0;
	PhysicsServer::AreaSpaceOverrideMode override_mode = PhysicsServer::AREA_SPACE_OVERRIDE_DISABLED;
	uint32_t collision_layer = 1;
	uint32_t collision_mask = 1;
	bool monitorable = false;
	bool ray_pickable = true;

	// Shape storage
	Vector<RID> shapes;
	Vector<Transform> shape_transforms;
	Vector<bool> shape_disabled;

	// Params
	real_t gravity = 9.81;
	Vector3 gravity_vector = Vector3(0, -1, 0);
	bool gravity_is_point = false;
	real_t gravity_distance_scale = 0.0;
	real_t gravity_point_attenuation = 0.0;
	real_t linear_damp = 0.1;
	real_t angular_damp = 0.1;
	real_t priority = 0.0;

	Transform transform;

	// Callbacks
	Object *monitor_callback_receiver = nullptr;
	StringName monitor_callback_method;
	Object *area_monitor_callback_receiver = nullptr;
	StringName area_monitor_callback_method;

	JoltAreaData() = default;
	~JoltAreaData() override = default;
};

#endif
