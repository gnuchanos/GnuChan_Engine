#include "jolt_space.h"
#include "jolt_physics_server.h"
#include "core/rid.h"
#include "core/print_string.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/TempAllocator.h>

// --- BroadPhaseLayerInterface implementation ---
namespace {
	enum BroadPhaseLayers {
		BROAD_PHASE_NON_MOVING = 0,
		BROAD_PHASE_MOVING = 1,
		NUM_BROAD_PHASE_LAYERS = 2
	};

	class BPLayerInterfaceImpl : public JPH::BroadPhaseLayerInterface {
	public:
		BPLayerInterfaceImpl() = default;
		virtual JPH::uint GetNumBroadPhaseLayers() const override { return NUM_BROAD_PHASE_LAYERS; }
		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
			return JPH::BroadPhaseLayer(inLayer == 0 ? BROAD_PHASE_NON_MOVING : BROAD_PHASE_MOVING);
		}
	};

	class ObjectVsBPLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer, JPH::BroadPhaseLayer) const override { return true; }
	};

	class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer, JPH::ObjectLayer) const override { return true; }
	};
}

JoltSpaceData::JoltSpaceData()
	: bp_layer_interface(new BPLayerInterfaceImpl())
	, obj_vs_bp(new ObjectVsBPLayerFilterImpl())
	, obj_pair(new ObjectLayerPairFilterImpl()) {
}

JoltSpaceData::~JoltSpaceData() {
	delete system; system = nullptr;
	delete static_cast<JPH::JobSystem *>(job_system); job_system = nullptr;
	delete static_cast<JPH::TempAllocator *>(temp_allocator); temp_allocator = nullptr;
	delete bp_layer_interface; bp_layer_interface = nullptr;
	delete obj_vs_bp; obj_vs_bp = nullptr;
	delete obj_pair; obj_pair = nullptr;
}

void JoltSpaceData::init() {
	const JPH::uint max_bodies = 65536;
	const JPH::uint num_body_mutexes = 0;
	const JPH::uint max_body_pairs = 65536;
	const JPH::uint max_contact_constraints = 10240;

	job_system = new JPH::JobSystemSingleThreaded();
	temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

	system = new JPH::PhysicsSystem();
	system->Init(max_bodies, num_body_mutexes, max_body_pairs, max_contact_constraints,
		*bp_layer_interface, *obj_vs_bp, *obj_pair);
}

void JoltSpaceData::step(float p_step, JoltPhysicsServer *) {
	if (!system || !active) return;
	system->Update(p_step, 1, static_cast<JPH::TempAllocator *>(temp_allocator), static_cast<JPH::JobSystem *>(job_system));
}

static JPH::Vec3 to_jvec3(const Vector3 &v) { return JPH::Vec3(v.x, v.y, v.z); }
static Vector3 to_vec3(const JPH::Vec3 &v) { return Vector3(v.GetX(), v.GetY(), v.GetZ()); }

JoltSpace::JoltSpace(JPH::PhysicsSystem *p_sys) : system(p_sys) {}
JoltSpace::~JoltSpace() {}

int JoltSpace::intersect_point(const Vector3 &, ShapeResult *, int, const Set<RID> &, uint32_t, bool, bool) { return 0; }

bool JoltSpace::intersect_ray(const Vector3 &p_from, const Vector3 &p_to, RayResult &r_result,
		const Set<RID> &, uint32_t, bool, bool, bool) {
	if (!system) return false;
	const JPH::NarrowPhaseQuery &npq = system->GetNarrowPhaseQuery();
	JPH::RRayCast ray(JPH::RVec3(to_jvec3(p_from)), to_jvec3(p_to - p_from));
	JPH::RayCastResult hit;
	if (npq.CastRay(ray, hit)) {
		JPH::BodyLockRead lock(system->GetBodyLockInterface(), hit.mBodyID);
		if (lock.Succeeded()) {
			const JPH::Body &body = lock.GetBody();
			JPH::RVec3 hit_pos = ray.GetPointOnRay(hit.mFraction);
			r_result.position = Vector3(real_t(hit_pos.GetX()), real_t(hit_pos.GetY()), real_t(hit_pos.GetZ()));
			r_result.normal = to_vec3(body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, hit_pos));
			r_result.rid = RID();
			r_result.collider_id = body.GetID().GetIndex();
			r_result.collider = nullptr;
			r_result.shape = hit.mSubShapeID2.GetValue();
			return true;
		}
	}
	return false;
}

int JoltSpace::intersect_shape(const RID &, const Transform &, float, ShapeResult *, int,
		const Set<RID> &, uint32_t, bool, bool) { return 0; }

bool JoltSpace::cast_motion(const RID &, const Transform &, const Vector3 &, float,
		float &, float &, const Set<RID> &, uint32_t, bool, bool, ShapeRestInfo *) { return false; }

bool JoltSpace::collide_shape(RID, const Transform &, float, Vector3 *, int, int &,
		const Set<RID> &, uint32_t, bool, bool) { return false; }

bool JoltSpace::rest_info(RID, const Transform &, float, ShapeRestInfo *,
		const Set<RID> &, uint32_t, bool, bool) { return false; }

Vector3 JoltSpace::get_closest_point_to_object_volume(RID, const Vector3) const { return Vector3(); }
