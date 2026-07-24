#ifndef JOLT_SPACE_H
#define JOLT_SPACE_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "core/rid.h"
#include "servers/physics_server.h"

namespace JPH {
class JobSystem;
class TempAllocator;
}

class JoltPhysicsServer;

class JoltSpaceData : public RID_Data {
public:
	JPH::PhysicsSystem *system = nullptr;
	JPH::JobSystem *job_system = nullptr;
	JPH::TempAllocator *temp_allocator = nullptr;
	bool active = true;
	Vector<Vector3> debug_contacts;

	JoltSpaceData();
	~JoltSpaceData() override;
	void init();
	void step(float p_step, JoltPhysicsServer *server);
};

// Direct space state for raycasting/intersection queries
class JoltSpace : public PhysicsDirectSpaceState {
	GDCLASS(JoltSpace, PhysicsDirectSpaceState);
public:
	JPH::PhysicsSystem *system = nullptr;

	JoltSpace(JPH::PhysicsSystem *p_sys = nullptr);
	~JoltSpace() override;

	virtual int intersect_point(const Vector3 &p_point, ShapeResult *r_results, int p_result_max, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = true) override;
	virtual bool intersect_ray(const Vector3 &p_from, const Vector3 &p_to, RayResult &r_result, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = true, bool p_pick_ray = false) override;
	virtual int intersect_shape(const RID &p_shape, const Transform &p_xform, float p_margin, ShapeResult *r_results, int p_result_max, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = true) override;
	virtual bool cast_motion(const RID &p_shape, const Transform &p_xform, const Vector3 &p_motion, float p_margin, float &r_closest_safe, float &r_closest_unsafe, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = true, ShapeRestInfo *r_info = nullptr) override;
	virtual bool collide_shape(RID p_shape, const Transform &p_shape_xform, float p_margin, Vector3 *r_results, int p_result_max, int &r_result_count, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = true) override;
	virtual bool rest_info(RID p_shape, const Transform &p_shape_xform, float p_margin, ShapeRestInfo *r_info, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = true) override;
	virtual Vector3 get_closest_point_to_object_volume(RID p_object, const Vector3 p_point) const override;
};

#endif
