#include "jolt_physics_server.h"
#include "jolt_shape.h"
#include "jolt_space.h"
#include "jolt_body.h"
#include "jolt_area.h"
#include "jolt_joint.h"
#include "core/rid.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/Constraints/SliderConstraint.h>
#include <Jolt/Physics/Constraints/ConeConstraint.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

JoltPhysicsServer *JoltPhysicsServer::singleton = nullptr;

PhysicsServer *JoltPhysicsServer::create_callback() {
	return memnew(JoltPhysicsServer);
}

JoltPhysicsServer::JoltPhysicsServer() {
	singleton = this;
	direct_state = nullptr;
	flushing_queries_flag = false;
	collision_iterations = 8;
}

JoltPhysicsServer::~JoltPhysicsServer() {
	singleton = nullptr;
}

// ========== Conversion helpers ==========
static JPH::Vec3 jv(const Vector3 &v) { return JPH::Vec3(v.x, v.y, v.z); }
static Vector3 gv(const JPH::Vec3 &v) { return Vector3(v.GetX(), v.GetY(), v.GetZ()); }
static JPH::Quat jq(const Quat &q) { return JPH::Quat(q.x, q.y, q.z, q.w); }
static Quat gq(const JPH::Quat &q) { return Quat(q.GetX(), q.GetY(), q.GetZ(), q.GetW()); }

static Transform gxf(JPH::RVec3Arg pos, JPH::QuatArg rot) {
	Transform t;
	t.origin = Vector3(real_t(pos.GetX()), real_t(pos.GetY()), real_t(pos.GetZ()));
	t.basis = Basis(gq(rot));
	return t;
}

static JPH::EMotionType mm(PhysicsServer::BodyMode m) {
	switch (m) {
		case PhysicsServer::BODY_MODE_STATIC: return JPH::EMotionType::Static;
		case PhysicsServer::BODY_MODE_KINEMATIC: return JPH::EMotionType::Kinematic;
		case PhysicsServer::BODY_MODE_RIGID: return JPH::EMotionType::Dynamic;
		case PhysicsServer::BODY_MODE_CHARACTER: return JPH::EMotionType::Dynamic;
	}
	return JPH::EMotionType::Static;
}

// ========== SHAPE API ==========
RID JoltPhysicsServer::shape_create(ShapeType p_shape) {
	JoltShapeData *sd = memnew(JoltShapeData(p_shape));
	return shape_owner.make_rid(sd);
}

void JoltPhysicsServer::shape_set_data(RID p_shape, const Variant &p_data) {
	JoltShapeData *sd = shape_owner.get(p_shape);
	if (!sd) return;
	sd->data = p_data;

	JPH::RefConst<JPH::Shape> js = nullptr;

	switch (sd->type) {
		case SHAPE_SPHERE: {
			float r = p_data;
			js = new JPH::SphereShape(r);
			break;
		}
		case SHAPE_BOX: {
			Vector3 e = p_data;
			js = new JPH::BoxShape(jv(e));
			break;
		}
		case SHAPE_CAPSULE: {
			if (p_data.get_type() == Variant::DICTIONARY) {
				Dictionary d = p_data;
				float r = d["radius"];
				float h = d["height"];
				js = new JPH::CapsuleShape(h * 0.5f, r);
			}
			break;
		}
		case SHAPE_CYLINDER: {
			if (p_data.get_type() == Variant::DICTIONARY) {
				Dictionary d = p_data;
				js = new JPH::CylinderShape((float)d["radius"], (float)d["height"] * 0.5f);
			}
			break;
		}
		case SHAPE_PLANE: {
			js = new JPH::BoxShape(JPH::Vec3(50, 0.01f, 50));
			break;
		}
		case SHAPE_RAY: {
			float len = p_data;
			js = new JPH::BoxShape(JPH::Vec3(0.001f, 0.001f, len * 0.5f));
			break;
		}
		case SHAPE_CONVEX_POLYGON: {
			PoolVector<Vector3> pts = p_data;
			PoolVector<Vector3>::Read r = pts.read();
			JPH::Array<JPH::Vec3> verts;
			for (int i = 0; i < pts.size(); i++) verts.push_back(jv(r[i]));
			JPH::ConvexHullShapeSettings cs(verts.data(), (int)verts.size());
			JPH::Shape::ShapeResult result = cs.Create();
			if (result.IsValid()) js = result.Get();
			break;
		}
		case SHAPE_CONCAVE_POLYGON:
		case SHAPE_HEIGHTMAP: {
			js = new JPH::BoxShape(JPH::Vec3(1, 1, 1));
			break;
		}
		default: break;
	}
	if (js) {
		sd->shape = js;
	}
}

void JoltPhysicsServer::shape_set_custom_solver_bias(RID p_shape, real_t p_bias) {
	JoltShapeData *sd = shape_owner.get(p_shape);
	if (sd) sd->custom_solver_bias = p_bias;
}

PhysicsServer::ShapeType JoltPhysicsServer::shape_get_type(RID p_shape) const {
	const JoltShapeData *sd = shape_owner.get(p_shape);
	if (sd) return sd->type;
	return SHAPE_CUSTOM;
}

Variant JoltPhysicsServer::shape_get_data(RID p_shape) const {
	const JoltShapeData *sd = shape_owner.get(p_shape);
	if (sd) return sd->data;
	return Variant();
}

void JoltPhysicsServer::shape_set_margin(RID p_shape, real_t p_margin) {
	JoltShapeData *sd = shape_owner.get(p_shape);
	if (sd) sd->margin = p_margin;
}

real_t JoltPhysicsServer::shape_get_margin(RID p_shape) const {
	const JoltShapeData *sd = shape_owner.get(p_shape);
	if (sd) return sd->margin;
	return 0.0;
}

real_t JoltPhysicsServer::shape_get_custom_solver_bias(RID p_shape) const {
	const JoltShapeData *sd = shape_owner.get(p_shape);
	if (sd) return sd->custom_solver_bias;
	return 0.0;
}

// ========== SPACE API ==========
RID JoltPhysicsServer::space_create() {
	JoltSpaceData *sd = memnew(JoltSpaceData());
	sd->init();
	return space_owner.make_rid(sd);
}

void JoltPhysicsServer::space_set_active(RID p_space, bool p_active) {
	JoltSpaceData *sd = space_owner.get(p_space);
	if (sd) sd->active = p_active;
}

bool JoltPhysicsServer::space_is_active(RID p_space) const {
	const JoltSpaceData *sd = space_owner.get(p_space);
	return sd && sd->active;
}

void JoltPhysicsServer::space_set_param(RID, SpaceParameter, real_t) {}
real_t JoltPhysicsServer::space_get_param(RID, SpaceParameter) const { return 0.0; }

PhysicsDirectSpaceState *JoltPhysicsServer::space_get_direct_state(RID p_space) {
	JoltSpaceData *sd = space_owner.get(p_space);
	if (!sd || !sd->system) return nullptr;
	space_state_instance.system = sd->system;
	return &space_state_instance;
}

void JoltPhysicsServer::space_set_debug_contacts(RID, int) {}
Vector<Vector3> JoltPhysicsServer::space_get_contacts(RID) const { return Vector<Vector3>(); }
int JoltPhysicsServer::space_get_contact_count(RID) const { return 0; }

// ========== AREA API ==========
RID JoltPhysicsServer::area_create() {
	JoltAreaData *ad = memnew(JoltAreaData());
	return area_owner.make_rid(ad);
}

void JoltPhysicsServer::area_set_space(RID p_area, RID p_space) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return;
	ad->space_rid = p_space;
	ad->space = p_space;
}

RID JoltPhysicsServer::area_get_space(RID p_area) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	return ad ? ad->space_rid : RID();
}

void JoltPhysicsServer::area_set_space_override_mode(RID p_area, AreaSpaceOverrideMode p_mode) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (ad) ad->override_mode = p_mode;
}

PhysicsServer::AreaSpaceOverrideMode JoltPhysicsServer::area_get_space_override_mode(RID p_area) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	return ad ? ad->override_mode : AREA_SPACE_OVERRIDE_DISABLED;
}

void JoltPhysicsServer::area_add_shape(RID p_area, RID p_shape, const Transform &p_t, bool p_d) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return;
	ad->shapes.push_back(p_shape);
	ad->shape_transforms.push_back(p_t);
	ad->shape_disabled.push_back(p_d);
}

void JoltPhysicsServer::area_set_shape(RID p_area, int idx, RID p_shape) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad || idx < 0 || idx >= ad->shapes.size()) return;
	ad->shapes.write[idx] = p_shape;
}

void JoltPhysicsServer::area_set_shape_transform(RID p_area, int idx, const Transform &p_t) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad || idx < 0 || idx >= ad->shape_transforms.size()) return;
	ad->shape_transforms.write[idx] = p_t;
}

int JoltPhysicsServer::area_get_shape_count(RID p_area) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	return ad ? ad->shapes.size() : 0;
}

RID JoltPhysicsServer::area_get_shape(RID p_area, int idx) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	if (!ad || idx < 0 || idx >= ad->shapes.size()) return RID();
	return ad->shapes[idx];
}

Transform JoltPhysicsServer::area_get_shape_transform(RID p_area, int idx) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	if (!ad || idx < 0 || idx >= ad->shape_transforms.size()) return Transform();
	return ad->shape_transforms[idx];
}

void JoltPhysicsServer::area_remove_shape(RID p_area, int idx) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad || idx < 0 || idx >= ad->shapes.size()) return;
	ad->shapes.remove(idx);
	ad->shape_transforms.remove(idx);
	ad->shape_disabled.remove(idx);
}

void JoltPhysicsServer::area_clear_shapes(RID p_area) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return;
	ad->shapes.clear();
	ad->shape_transforms.clear();
	ad->shape_disabled.clear();
}

void JoltPhysicsServer::area_set_shape_disabled(RID p_area, int idx, bool p_d) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad || idx < 0 || idx >= ad->shape_disabled.size()) return;
	ad->shape_disabled.write[idx] = p_d;
}

void JoltPhysicsServer::area_attach_object_instance_id(RID p_area, ObjectID p_id) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (ad) ad->instance_id = p_id;
}

ObjectID JoltPhysicsServer::area_get_object_instance_id(RID p_area) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	return ad ? ad->instance_id : ObjectID(0);
}

void JoltPhysicsServer::area_set_param(RID p_area, AreaParameter p, const Variant &v) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return;
	switch (p) {
		case AREA_PARAM_GRAVITY: ad->gravity = v; break;
		case AREA_PARAM_GRAVITY_VECTOR: ad->gravity_vector = v; break;
		case AREA_PARAM_GRAVITY_IS_POINT: ad->gravity_is_point = v; break;
		case AREA_PARAM_GRAVITY_DISTANCE_SCALE: ad->gravity_distance_scale = v; break;
		case AREA_PARAM_GRAVITY_POINT_ATTENUATION: ad->gravity_point_attenuation = v; break;
		case AREA_PARAM_LINEAR_DAMP: ad->linear_damp = v; break;
		case AREA_PARAM_ANGULAR_DAMP: ad->angular_damp = v; break;
		case AREA_PARAM_PRIORITY: ad->priority = v; break;
	}
}

void JoltPhysicsServer::area_set_transform(RID p_area, const Transform &p_t) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (ad) ad->transform = p_t;
}

Variant JoltPhysicsServer::area_get_param(RID p_area, AreaParameter p) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return Variant();
	switch (p) {
		case AREA_PARAM_GRAVITY: return ad->gravity;
		case AREA_PARAM_GRAVITY_VECTOR: return ad->gravity_vector;
		case AREA_PARAM_GRAVITY_IS_POINT: return ad->gravity_is_point;
		case AREA_PARAM_GRAVITY_DISTANCE_SCALE: return ad->gravity_distance_scale;
		case AREA_PARAM_GRAVITY_POINT_ATTENUATION: return ad->gravity_point_attenuation;
		case AREA_PARAM_LINEAR_DAMP: return ad->linear_damp;
		case AREA_PARAM_ANGULAR_DAMP: return ad->angular_damp;
		case AREA_PARAM_PRIORITY: return ad->priority;
	}
	return Variant();
}

Transform JoltPhysicsServer::area_get_transform(RID p_area) const {
	const JoltAreaData *ad = area_owner.get(p_area);
	return ad ? ad->transform : Transform();
}

void JoltPhysicsServer::area_set_collision_mask(RID p_area, uint32_t m) { JoltAreaData *ad = area_owner.get(p_area); if (ad) ad->collision_mask = m; }
void JoltPhysicsServer::area_set_collision_layer(RID p_area, uint32_t l) { JoltAreaData *ad = area_owner.get(p_area); if (ad) ad->collision_layer = l; }
void JoltPhysicsServer::area_set_monitorable(RID p_area, bool b) { JoltAreaData *ad = area_owner.get(p_area); if (ad) ad->monitorable = b; }

void JoltPhysicsServer::area_set_monitor_callback(RID p_area, Object *r, const StringName &m) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return;
	ad->monitor_callback_receiver = r;
	ad->monitor_callback_method = m;
}

void JoltPhysicsServer::area_set_area_monitor_callback(RID p_area, Object *r, const StringName &m) {
	JoltAreaData *ad = area_owner.get(p_area);
	if (!ad) return;
	ad->area_monitor_callback_receiver = r;
	ad->area_monitor_callback_method = m;
}

void JoltPhysicsServer::area_set_ray_pickable(RID p_area, bool e) { JoltAreaData *ad = area_owner.get(p_area); if (ad) ad->ray_pickable = e; }
bool JoltPhysicsServer::area_is_ray_pickable(RID p_area) const { const JoltAreaData *ad = area_owner.get(p_area); return ad && ad->ray_pickable; }

// ========== BODY API ==========
RID JoltPhysicsServer::body_create(BodyMode p_mode, bool p_init_sleeping) {
	JoltBodyData *bd = memnew(JoltBodyData());
	bd->mode = p_mode;
	bd->init_sleeping = p_init_sleeping;
	return body_owner.make_rid(bd);
}

void JoltPhysicsServer::body_set_space(RID p_body, RID p_space) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	bd->space_rid = p_space;

	JoltSpaceData *sd = space_owner.get(p_space);
	if (!sd || !sd->system) return;

	JPH::BodyInterface &bi = sd->system->GetBodyInterface();
	if (!bd->body_id.IsInvalid()) {
		bi.RemoveBody(bd->body_id);
		bi.DestroyBody(bd->body_id);
		bd->body_id = JPH::BodyID();
	}

	if (bd->shapes.size() == 0) return;

	JPH::RefConst<JPH::Shape> final_shape = nullptr;
	if (bd->shapes.size() == 1) {
		JoltShapeData *sd_shape = shape_owner.get(bd->shapes[0]);
		if (sd_shape && sd_shape->shape.GetPtr() != nullptr) {
			if (bd->shape_transforms[0] == Transform()) {
				final_shape = sd_shape->shape;
			} else {
				JPH::RotatedTranslatedShapeSettings rt(
					jv(bd->shape_transforms[0].origin),
					jq(bd->shape_transforms[0].basis.get_quat()),
					sd_shape->shape.GetPtr());
				JPH::Shape::ShapeResult r = rt.Create();
				if (r.IsValid()) final_shape = r.Get();
			}
		}
	} else {
		JPH::StaticCompoundShapeSettings compound;
		for (int i = 0; i < bd->shapes.size(); i++) {
			JoltShapeData *sd_shape = shape_owner.get(bd->shapes[i]);
			if (sd_shape && sd_shape->shape.GetPtr() != nullptr) {
				compound.AddShape(
					jv(bd->shape_transforms[i].origin),
					jq(bd->shape_transforms[i].basis.get_quat()),
					sd_shape->shape.GetPtr());
			}
		}
		JPH::Shape::ShapeResult r = compound.Create();
		if (r.IsValid()) final_shape = r.Get();
	}

	if (!final_shape) return;

	JPH::BodyCreationSettings bcs(final_shape, JPH::RVec3::sZero(), JPH::Quat::sIdentity(), mm(bd->mode), 1);
	bcs.mFriction = bd->friction;
	bcs.mRestitution = bd->bounce;
	bcs.mLinearDamping = bd->linear_damp;
	bcs.mAngularDamping = bd->angular_damp;
	bcs.mGravityFactor = bd->gravity_scale;
	bcs.mAllowSleeping = !bd->init_sleeping;
	bcs.mMotionQuality = bd->ccd_enabled ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete;

	JPH::Body *body = bi.CreateBody(bcs);
	if (body) {
		bd->body_id = body->GetID();
		bi.AddBody(bd->body_id, bd->init_sleeping ? JPH::EActivation::DontActivate : JPH::EActivation::Activate);
	}
}

RID JoltPhysicsServer::body_get_space(RID p_body) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	return bd ? bd->space_rid : RID();
}

void JoltPhysicsServer::body_set_mode(RID p_body, BodyMode p_mode) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	bd->mode = p_mode;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().SetMotionType(bd->body_id, mm(p_mode), JPH::EActivation::Activate);
}

PhysicsServer::BodyMode JoltPhysicsServer::body_get_mode(RID p_body) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	return bd ? bd->mode : BODY_MODE_STATIC;
}

void JoltPhysicsServer::body_add_shape(RID p_body, RID p_shape, const Transform &p_t, bool p_d) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	bd->shapes.push_back(p_shape);
	bd->shape_transforms.push_back(p_t);
	bd->shape_disabled.push_back(p_d);
}

void JoltPhysicsServer::body_set_shape(RID p_body, int idx, RID p_shape) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd || idx < 0 || idx >= bd->shapes.size()) return;
	bd->shapes.write[idx] = p_shape;
}

void JoltPhysicsServer::body_set_shape_transform(RID p_body, int idx, const Transform &p_t) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd || idx < 0 || idx >= bd->shape_transforms.size()) return;
	bd->shape_transforms.write[idx] = p_t;
}

int JoltPhysicsServer::body_get_shape_count(RID p_body) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	return bd ? bd->shapes.size() : 0;
}

RID JoltPhysicsServer::body_get_shape(RID p_body, int idx) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	if (!bd || idx < 0 || idx >= bd->shapes.size()) return RID();
	return bd->shapes[idx];
}

Transform JoltPhysicsServer::body_get_shape_transform(RID p_body, int idx) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	if (!bd || idx < 0 || idx >= bd->shape_transforms.size()) return Transform();
	return bd->shape_transforms[idx];
}

void JoltPhysicsServer::body_remove_shape(RID p_body, int idx) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd || idx < 0 || idx >= bd->shapes.size()) return;
	bd->shapes.remove(idx);
	bd->shape_transforms.remove(idx);
	bd->shape_disabled.remove(idx);
}

void JoltPhysicsServer::body_clear_shapes(RID p_body) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	bd->shapes.clear();
	bd->shape_transforms.clear();
	bd->shape_disabled.clear();
}

void JoltPhysicsServer::body_set_shape_disabled(RID p_body, int idx, bool p_d) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd || idx < 0 || idx >= bd->shape_disabled.size()) return;
	bd->shape_disabled.write[idx] = p_d;
}

void JoltPhysicsServer::body_attach_object_instance_id(RID p_body, uint32_t pid) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->instance_id = pid; }
uint32_t JoltPhysicsServer::body_get_object_instance_id(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->instance_id : 0; }

void JoltPhysicsServer::body_set_enable_continuous_collision_detection(RID p_body, bool p_e) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	bd->ccd_enabled = p_e;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().SetMotionQuality(bd->body_id,
			p_e ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete);
}

bool JoltPhysicsServer::body_is_continuous_collision_detection_enabled(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd && bd->ccd_enabled; }

void JoltPhysicsServer::body_set_collision_layer(RID p_body, uint32_t l) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->collision_layer = l; }
uint32_t JoltPhysicsServer::body_get_collision_layer(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->collision_layer : 0; }
void JoltPhysicsServer::body_set_collision_mask(RID p_body, uint32_t m) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->collision_mask = m; }
uint32_t JoltPhysicsServer::body_get_collision_mask(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->collision_mask : 0; }
void JoltPhysicsServer::body_set_user_flags(RID p_body, uint32_t f) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->user_flags = f; }
uint32_t JoltPhysicsServer::body_get_user_flags(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->user_flags : 0; }

void JoltPhysicsServer::body_set_param(RID p_body, BodyParameter p, float v) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	JPH::BodyInterface *bi = (sd && sd->system) ? &sd->system->GetBodyInterface() : nullptr;
	switch (p) {
		case BODY_PARAM_BOUNCE:
			bd->bounce = v;
			if (bi && !bd->body_id.IsInvalid()) bi->SetRestitution(bd->body_id, v);
			break;
		case BODY_PARAM_FRICTION:
			bd->friction = v;
			if (bi && !bd->body_id.IsInvalid()) bi->SetFriction(bd->body_id, v);
			break;
		case BODY_PARAM_MASS: bd->mass = v; break;
		case BODY_PARAM_GRAVITY_SCALE:
			bd->gravity_scale = v;
			if (bi && !bd->body_id.IsInvalid()) bi->SetGravityFactor(bd->body_id, v);
			break;
		case BODY_PARAM_LINEAR_DAMP: bd->linear_damp = v; break;
		case BODY_PARAM_ANGULAR_DAMP: bd->angular_damp = v; break;
		case BODY_PARAM_MAX: break;
	}
}

float JoltPhysicsServer::body_get_param(RID p_body, BodyParameter p) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return 0.0;
	switch (p) {
		case BODY_PARAM_BOUNCE: return bd->bounce;
		case BODY_PARAM_FRICTION: return bd->friction;
		case BODY_PARAM_MASS: return bd->mass;
		case BODY_PARAM_GRAVITY_SCALE: return bd->gravity_scale;
		case BODY_PARAM_LINEAR_DAMP: return bd->linear_damp;
		case BODY_PARAM_ANGULAR_DAMP: return bd->angular_damp;
		case BODY_PARAM_MAX: return 0.0;
	}
	return 0.0;
}

void JoltPhysicsServer::body_set_kinematic_safe_margin(RID p_body, real_t m) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->kinematic_safe_margin = m; }
real_t JoltPhysicsServer::body_get_kinematic_safe_margin(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->kinematic_safe_margin : 0.0; }

void JoltPhysicsServer::body_set_state(RID p_body, BodyState s, const Variant &v) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (!sd || !sd->system || bd->body_id.IsInvalid()) return;
	JPH::BodyInterface &bi = sd->system->GetBodyInterface();
	switch (s) {
		case BODY_STATE_TRANSFORM: {
			Transform t = v;
			bi.SetPositionAndRotation(bd->body_id, JPH::RVec3(jv(t.origin)), jq(t.basis.get_quat()), JPH::EActivation::Activate);
			break;
		}
		case BODY_STATE_LINEAR_VELOCITY:
			bi.SetLinearVelocity(bd->body_id, jv((Vector3)v));
			break;
		case BODY_STATE_ANGULAR_VELOCITY:
			bi.SetAngularVelocity(bd->body_id, jv((Vector3)v));
			break;
		case BODY_STATE_SLEEPING:
			if (v.operator bool()) bi.DeactivateBody(bd->body_id);
			else bi.ActivateBody(bd->body_id);
			break;
		case BODY_STATE_CAN_SLEEP: break;
	}
}

Variant JoltPhysicsServer::body_get_state(RID p_body, BodyState s) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return Variant();
	const JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (!sd || !sd->system || bd->body_id.IsInvalid()) return Variant();
	const JPH::BodyInterface &bi = sd->system->GetBodyInterface();
	switch (s) {
		case BODY_STATE_TRANSFORM: {
			JPH::RVec3 pos; JPH::Quat rot;
			bi.GetPositionAndRotation(bd->body_id, pos, rot);
			return gxf(pos, rot);
		}
		case BODY_STATE_LINEAR_VELOCITY: return gv(bi.GetLinearVelocity(bd->body_id));
		case BODY_STATE_ANGULAR_VELOCITY: return gv(bi.GetAngularVelocity(bd->body_id));
		case BODY_STATE_SLEEPING: return !bi.IsActive(bd->body_id);
		case BODY_STATE_CAN_SLEEP: return true;
	}
	return Variant();
}

void JoltPhysicsServer::body_set_applied_force(RID, const Vector3 &) {}
Vector3 JoltPhysicsServer::body_get_applied_force(RID) const { return Vector3(); }
void JoltPhysicsServer::body_set_applied_torque(RID, const Vector3 &) {}
Vector3 JoltPhysicsServer::body_get_applied_torque(RID) const { return Vector3(); }

void JoltPhysicsServer::body_add_central_force(RID p_body, const Vector3 &p_f) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().AddForce(bd->body_id, jv(p_f));
}

void JoltPhysicsServer::body_add_force(RID p_body, const Vector3 &p_f, const Vector3 &p_pos) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().AddForce(bd->body_id, jv(p_f), JPH::RVec3(jv(p_pos)));
}

void JoltPhysicsServer::body_add_torque(RID p_body, const Vector3 &p_t) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().AddTorque(bd->body_id, jv(p_t));
}

void JoltPhysicsServer::body_apply_central_impulse(RID p_body, const Vector3 &p_i) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().AddImpulse(bd->body_id, jv(p_i));
}

void JoltPhysicsServer::body_apply_impulse(RID p_body, const Vector3 &p_pos, const Vector3 &p_i) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().AddImpulse(bd->body_id, jv(p_i), JPH::RVec3(jv(p_pos)));
}

void JoltPhysicsServer::body_apply_torque_impulse(RID p_body, const Vector3 &p_i) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid())
		sd->system->GetBodyInterface().AddAngularImpulse(bd->body_id, jv(p_i));
}

void JoltPhysicsServer::body_set_axis_velocity(RID p_body, const Vector3 &p_av) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	JoltSpaceData *sd = space_owner.get(bd->space_rid);
	if (sd && sd->system && !bd->body_id.IsInvalid()) {
		JPH::BodyInterface &bi = sd->system->GetBodyInterface();
		JPH::Vec3 cur = bi.GetLinearVelocity(bd->body_id);
		JPH::Vec3 axis = jv(p_av).Normalized();
		float speed = jv(p_av).Length();
		float cur_ax = cur.Dot(axis);
		bi.SetLinearVelocity(bd->body_id, cur + axis * (speed - cur_ax));
	}
}

void JoltPhysicsServer::body_set_axis_lock(RID p_body, BodyAxis ax, bool p_l) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	if (p_l) bd->axis_locks |= (uint32_t)ax;
	else bd->axis_locks &= ~(uint32_t)ax;
}

bool JoltPhysicsServer::body_is_axis_locked(RID p_body, BodyAxis ax) const {
	const JoltBodyData *bd = body_owner.get(p_body);
	return bd && (bd->axis_locks & (uint32_t)ax);
}

void JoltPhysicsServer::body_add_collision_exception(RID p_body, RID p_other) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->collision_exceptions.insert(p_other); }
void JoltPhysicsServer::body_remove_collision_exception(RID p_body, RID p_other) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->collision_exceptions.erase(p_other); }

void JoltPhysicsServer::body_get_collision_exceptions(RID p_body, List<RID> *p_ex) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	for (const Set<RID>::Element *E = bd->collision_exceptions.front(); E; E = E->next())
		p_ex->push_back(E->get());
}

void JoltPhysicsServer::body_set_max_contacts_reported(RID p_body, int c) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->max_contacts_reported = c; }
int JoltPhysicsServer::body_get_max_contacts_reported(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->max_contacts_reported : 0; }
void JoltPhysicsServer::body_set_contacts_reported_depth_threshold(RID p_body, float t) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->contacts_depth_threshold = t; }
float JoltPhysicsServer::body_get_contacts_reported_depth_threshold(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd ? bd->contacts_depth_threshold : 0.0; }
void JoltPhysicsServer::body_set_omit_force_integration(RID p_body, bool o) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->omit_force_integration = o; }
bool JoltPhysicsServer::body_is_omitting_force_integration(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd && bd->omit_force_integration; }

void JoltPhysicsServer::body_set_force_integration_callback(RID p_body, Object *r, const StringName &m, const Variant &u) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return;
	bd->force_integration_cb_receiver = r;
	bd->force_integration_cb_method = m;
	bd->force_integration_cb_udata = u;
}

void JoltPhysicsServer::body_set_ray_pickable(RID p_body, bool e) { JoltBodyData *bd = body_owner.get(p_body); if (bd) bd->ray_pickable = e; }
bool JoltPhysicsServer::body_is_ray_pickable(RID p_body) const { const JoltBodyData *bd = body_owner.get(p_body); return bd && bd->ray_pickable; }

PhysicsDirectBodyState *JoltPhysicsServer::body_get_direct_state(RID p_body) {
	JoltBodyData *bd = body_owner.get(p_body);
	if (!bd) return nullptr;
	if (!bd->direct_state) bd->direct_state = memnew(JoltPhysicsDirectBodyState);
	bd->direct_state->body_data = bd;
	return bd->direct_state;
}

bool JoltPhysicsServer::body_test_motion(RID, const Transform &, const Vector3 &, bool, MotionResult *, bool, const Set<RID> &) { return false; }
int JoltPhysicsServer::body_test_ray_separation(RID, const Transform &, bool, Vector3 &, SeparationResult *, int, float) { return 0; }

// ========== SOFT BODY API ==========
RID JoltPhysicsServer::soft_body_create(bool p_init_sleeping) {
	JoltBodyData *bd = memnew(JoltBodyData());
	bd->mode = BODY_MODE_RIGID;
	bd->init_sleeping = p_init_sleeping;
	return soft_body_owner.make_rid(bd);
}
void JoltPhysicsServer::soft_body_update_visual_server(RID, class SoftBodyVisualServerHandler *) {}
void JoltPhysicsServer::soft_body_set_space(RID p_body, RID p_space) { JoltBodyData *bd = soft_body_owner.get(p_body); if (bd) bd->space_rid = p_space; }
RID JoltPhysicsServer::soft_body_get_space(RID p_body) const { const JoltBodyData *bd = soft_body_owner.get(p_body); return bd ? bd->space_rid : RID(); }
void JoltPhysicsServer::soft_body_set_mesh(RID, const REF &) {}
void JoltPhysicsServer::soft_body_set_collision_layer(RID p_body, uint32_t l) { JoltBodyData *bd = soft_body_owner.get(p_body); if (bd) bd->collision_layer = l; }
uint32_t JoltPhysicsServer::soft_body_get_collision_layer(RID p_body) const { const JoltBodyData *bd = soft_body_owner.get(p_body); return bd ? bd->collision_layer : 0; }
void JoltPhysicsServer::soft_body_set_collision_mask(RID p_body, uint32_t m) { JoltBodyData *bd = soft_body_owner.get(p_body); if (bd) bd->collision_mask = m; }
uint32_t JoltPhysicsServer::soft_body_get_collision_mask(RID p_body) const { const JoltBodyData *bd = soft_body_owner.get(p_body); return bd ? bd->collision_mask : 0; }
void JoltPhysicsServer::soft_body_add_collision_exception(RID p_body, RID p_b) { JoltBodyData *bd = soft_body_owner.get(p_body); if (bd) bd->collision_exceptions.insert(p_b); }
void JoltPhysicsServer::soft_body_remove_collision_exception(RID p_body, RID p_b) { JoltBodyData *bd = soft_body_owner.get(p_body); if (bd) bd->collision_exceptions.erase(p_b); }
void JoltPhysicsServer::soft_body_get_collision_exceptions(RID p_body, List<RID> *p_ex) { JoltBodyData *bd = soft_body_owner.get(p_body); if (!bd) return; for (const Set<RID>::Element *E = bd->collision_exceptions.front(); E; E = E->next()) p_ex->push_back(E->get()); }
void JoltPhysicsServer::soft_body_set_state(RID p_body, BodyState s, const Variant &v) { body_set_state(p_body, s, v); }
Variant JoltPhysicsServer::soft_body_get_state(RID p_body, BodyState s) const { return body_get_state(p_body, s); }
void JoltPhysicsServer::soft_body_set_transform(RID p_body, const Transform &t) { body_set_state(p_body, BODY_STATE_TRANSFORM, t); }
Vector3 JoltPhysicsServer::soft_body_get_vertex_position(RID, int) const { return Vector3(); }
void JoltPhysicsServer::soft_body_set_ray_pickable(RID p_body, bool e) { JoltBodyData *bd = soft_body_owner.get(p_body); if (bd) bd->ray_pickable = e; }
bool JoltPhysicsServer::soft_body_is_ray_pickable(RID p_body) const { const JoltBodyData *bd = soft_body_owner.get(p_body); return bd && bd->ray_pickable; }
void JoltPhysicsServer::soft_body_set_simulation_precision(RID, int) {}
int JoltPhysicsServer::soft_body_get_simulation_precision(RID) { return 5; }
void JoltPhysicsServer::soft_body_set_total_mass(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_total_mass(RID) { return 1.0; }
void JoltPhysicsServer::soft_body_set_linear_stiffness(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_linear_stiffness(RID) { return 0.5; }
void JoltPhysicsServer::soft_body_set_areaAngular_stiffness(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_areaAngular_stiffness(RID) { return 0.5; }
void JoltPhysicsServer::soft_body_set_volume_stiffness(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_volume_stiffness(RID) { return 0.5; }
void JoltPhysicsServer::soft_body_set_pressure_coefficient(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_pressure_coefficient(RID) { return 0.0; }
void JoltPhysicsServer::soft_body_set_pose_matching_coefficient(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_pose_matching_coefficient(RID) { return 0.0; }
void JoltPhysicsServer::soft_body_set_damping_coefficient(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_damping_coefficient(RID) { return 0.0; }
void JoltPhysicsServer::soft_body_set_drag_coefficient(RID, real_t) {}
real_t JoltPhysicsServer::soft_body_get_drag_coefficient(RID) { return 0.0; }
void JoltPhysicsServer::soft_body_move_point(RID, int, const Vector3 &) {}
Vector3 JoltPhysicsServer::soft_body_get_point_global_position(RID, int) { return Vector3(); }
Vector3 JoltPhysicsServer::soft_body_get_point_offset(RID, int) const { return Vector3(); }
void JoltPhysicsServer::soft_body_remove_all_pinned_points(RID) {}
void JoltPhysicsServer::soft_body_pin_point(RID, int, bool) {}
bool JoltPhysicsServer::soft_body_is_point_pinned(RID, int) { return false; }

// ========== JOINT API ==========
PhysicsServer::JointType JoltPhysicsServer::joint_get_type(RID p_j) const {
	const JoltJointData *jd = joint_owner.get(p_j);
	return jd ? jd->type : JOINT_PIN;
}

void JoltPhysicsServer::joint_set_solver_priority(RID p_j, int p) { JoltJointData *jd = joint_owner.get(p_j); if (jd) jd->solver_priority = p; }
int JoltPhysicsServer::joint_get_solver_priority(RID p_j) const { const JoltJointData *jd = joint_owner.get(p_j); return jd ? jd->solver_priority : 1; }

void JoltPhysicsServer::joint_disable_collisions_between_bodies(RID p_j, const bool p_d) {
	JoltJointData *jd = joint_owner.get(p_j);
	if (jd) jd->disable_collisions = p_d;
}

bool JoltPhysicsServer::joint_is_disabled_collisions_between_bodies(RID p_j) const {
	const JoltJointData *jd = joint_owner.get(p_j);
	return jd && jd->disable_collisions;
}

RID JoltPhysicsServer::joint_create_pin(RID p_body_A, const Vector3 &p_local_A, RID p_body_B, const Vector3 &p_local_B) {
	JoltBodyData *bd_a = body_owner.get(p_body_A);
	JoltBodyData *bd_b = body_owner.get(p_body_B);
	if (!bd_a || !bd_b) return RID();

	JoltJointData *jd = memnew(JoltJointData(JOINT_PIN));
	jd->body_a = p_body_A; jd->body_b = p_body_B;
	jd->pin_local_a = p_local_A; jd->pin_local_b = p_local_B;
	RID rid = joint_owner.make_rid(jd);

	JoltSpaceData *sd_a = space_owner.get(bd_a->space_rid);
	if (sd_a && sd_a->system && !bd_a->body_id.IsInvalid() && !bd_b->body_id.IsInvalid()) {
		JPH::BodyLockWrite la(sd_a->system->GetBodyLockInterface(), bd_a->body_id);
		JPH::BodyLockWrite lb(sd_a->system->GetBodyLockInterface(), bd_b->body_id);
		if (la.Succeeded() && lb.Succeeded()) {
			JPH::PointConstraintSettings s;
			s.mPoint1 = JPH::RVec3(jv(p_local_A));
			s.mPoint2 = JPH::RVec3(jv(p_local_B));
			s.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
			jd->constraint = static_cast<JPH::TwoBodyConstraint *>(s.Create(la.GetBody(), lb.GetBody()));
			if (jd->constraint) sd_a->system->AddConstraint(jd->constraint);
		}
	}
	return rid;
}

void JoltPhysicsServer::pin_joint_set_param(RID p_j, PinJointParam p, float v) {
	JoltJointData *jd = joint_owner.get(p_j);
	if (!jd) return;
	switch (p) { case PIN_JOINT_BIAS: jd->pin_bias = v; break; case PIN_JOINT_DAMPING: jd->pin_damping = v; break; case PIN_JOINT_IMPULSE_CLAMP: jd->pin_impulse_clamp = v; break; }
}
float JoltPhysicsServer::pin_joint_get_param(RID p_j, PinJointParam p) const {
	const JoltJointData *jd = joint_owner.get(p_j);
	if (!jd) return 0.0;
	switch (p) { case PIN_JOINT_BIAS: return jd->pin_bias; case PIN_JOINT_DAMPING: return jd->pin_damping; case PIN_JOINT_IMPULSE_CLAMP: return jd->pin_impulse_clamp; }
	return 0.0;
}
void JoltPhysicsServer::pin_joint_set_local_a(RID p_j, const Vector3 &a) { JoltJointData *jd = joint_owner.get(p_j); if (jd) jd->pin_local_a = a; }
Vector3 JoltPhysicsServer::pin_joint_get_local_a(RID p_j) const { const JoltJointData *jd = joint_owner.get(p_j); return jd ? jd->pin_local_a : Vector3(); }
void JoltPhysicsServer::pin_joint_set_local_b(RID p_j, const Vector3 &b) { JoltJointData *jd = joint_owner.get(p_j); if (jd) jd->pin_local_b = b; }
Vector3 JoltPhysicsServer::pin_joint_get_local_b(RID p_j) const { const JoltJointData *jd = joint_owner.get(p_j); return jd ? jd->pin_local_b : Vector3(); }

RID JoltPhysicsServer::joint_create_hinge(RID p_body_A, const Transform &p_hinge_A, RID p_body_B, const Transform &p_hinge_B) {
	JoltBodyData *bd_a = body_owner.get(p_body_A), *bd_b = body_owner.get(p_body_B);
	if (!bd_a || !bd_b) return RID();
	JoltJointData *jd = memnew(JoltJointData(JOINT_HINGE));
	jd->body_a = p_body_A; jd->body_b = p_body_B;
	jd->hinge_ref_a = p_hinge_A; jd->hinge_ref_b = p_hinge_B;
	RID rid = joint_owner.make_rid(jd);

	JoltSpaceData *sd_a = space_owner.get(bd_a->space_rid);
	if (sd_a && sd_a->system && !bd_a->body_id.IsInvalid() && !bd_b->body_id.IsInvalid()) {
		JPH::BodyLockWrite la(sd_a->system->GetBodyLockInterface(), bd_a->body_id);
		JPH::BodyLockWrite lb(sd_a->system->GetBodyLockInterface(), bd_b->body_id);
		if (la.Succeeded() && lb.Succeeded()) {
			JPH::HingeConstraintSettings s;
			s.mPoint1 = JPH::RVec3(jv(p_hinge_A.origin));
			s.mHingeAxis1 = jv(p_hinge_A.basis.get_axis(1));
			s.mNormalAxis1 = jv(p_hinge_A.basis.get_axis(0));
			s.mPoint2 = JPH::RVec3(jv(p_hinge_B.origin));
			s.mHingeAxis2 = jv(p_hinge_B.basis.get_axis(1));
			s.mNormalAxis2 = jv(p_hinge_B.basis.get_axis(0));
			s.mLimitsMin = jd->hinge_limit_lower;
			s.mLimitsMax = jd->hinge_limit_upper;
			jd->constraint = static_cast<JPH::TwoBodyConstraint *>(s.Create(la.GetBody(), lb.GetBody()));
			if (jd->constraint) sd_a->system->AddConstraint(jd->constraint);
		}
	}
	return rid;
}

RID JoltPhysicsServer::joint_create_hinge_simple(RID p_body_A, const Vector3 &p_pivot_A, const Vector3 &, RID p_body_B, const Vector3 &p_pivot_B, const Vector3 &) {
	Transform ta, tb; ta.origin = p_pivot_A; tb.origin = p_pivot_B;
	return joint_create_hinge(p_body_A, ta, p_body_B, tb);
}

void JoltPhysicsServer::hinge_joint_set_param(RID p_j, HingeJointParam p, float v) {
	JoltJointData *jd = joint_owner.get(p_j); if (!jd) return;
	switch (p) {
		case HINGE_JOINT_BIAS: jd->hinge_bias = v; break;
		case HINGE_JOINT_LIMIT_UPPER: jd->hinge_limit_upper = v; break;
		case HINGE_JOINT_LIMIT_LOWER: jd->hinge_limit_lower = v; break;
		case HINGE_JOINT_LIMIT_BIAS: jd->hinge_limit_bias = v; break;
		case HINGE_JOINT_LIMIT_SOFTNESS: jd->hinge_limit_softness = v; break;
		case HINGE_JOINT_LIMIT_RELAXATION: jd->hinge_limit_relaxation = v; break;
		case HINGE_JOINT_MOTOR_TARGET_VELOCITY: jd->hinge_motor_target_vel = v; break;
		case HINGE_JOINT_MOTOR_MAX_IMPULSE: jd->hinge_motor_max_impulse = v; break;
		case HINGE_JOINT_MAX: break;
	}
}

float JoltPhysicsServer::hinge_joint_get_param(RID p_j, HingeJointParam p) const {
	const JoltJointData *jd = joint_owner.get(p_j); if (!jd) return 0.0;
	switch (p) {
		case HINGE_JOINT_BIAS: return jd->hinge_bias;
		case HINGE_JOINT_LIMIT_UPPER: return jd->hinge_limit_upper;
		case HINGE_JOINT_LIMIT_LOWER: return jd->hinge_limit_lower;
		case HINGE_JOINT_LIMIT_BIAS: return jd->hinge_limit_bias;
		case HINGE_JOINT_LIMIT_SOFTNESS: return jd->hinge_limit_softness;
		case HINGE_JOINT_LIMIT_RELAXATION: return jd->hinge_limit_relaxation;
		case HINGE_JOINT_MOTOR_TARGET_VELOCITY: return jd->hinge_motor_target_vel;
		case HINGE_JOINT_MOTOR_MAX_IMPULSE: return jd->hinge_motor_max_impulse;
		case HINGE_JOINT_MAX: return 0.0;
	}
	return 0.0;
}

void JoltPhysicsServer::hinge_joint_set_flag(RID p_j, HingeJointFlag f, bool v) {
	JoltJointData *jd = joint_owner.get(p_j); if (!jd) return;
	switch (f) {
		case HINGE_JOINT_FLAG_USE_LIMIT: jd->hinge_use_limits = v; break;
		case HINGE_JOINT_FLAG_ENABLE_MOTOR: jd->hinge_enable_motor = v; break;
		case HINGE_JOINT_FLAG_MAX: break;
	}
	if (jd->constraint && jd->type == JOINT_HINGE) {
		JPH::HingeConstraint *hc = static_cast<JPH::HingeConstraint *>(jd->constraint);
		if (f == HINGE_JOINT_FLAG_USE_LIMIT) {
			if (v) hc->SetLimits(jd->hinge_limit_lower, jd->hinge_limit_upper);
			else hc->SetLimits(-3.14159f, 3.14159f);
		} else if (f == HINGE_JOINT_FLAG_ENABLE_MOTOR) {
			hc->SetMotorState(v ? JPH::EMotorState::Velocity : JPH::EMotorState::Off);
			hc->SetTargetAngularVelocity(jd->hinge_motor_target_vel);
		}
	}
}

bool JoltPhysicsServer::hinge_joint_get_flag(RID p_j, HingeJointFlag f) const {
	const JoltJointData *jd = joint_owner.get(p_j); if (!jd) return false;
	switch (f) { case HINGE_JOINT_FLAG_USE_LIMIT: return jd->hinge_use_limits; case HINGE_JOINT_FLAG_ENABLE_MOTOR: return jd->hinge_enable_motor; case HINGE_JOINT_FLAG_MAX: return false; }
	return false;
}

RID JoltPhysicsServer::joint_create_slider(RID p_body_A, const Transform &p_a, RID p_body_B, const Transform &p_b) {
	JoltBodyData *bd_a = body_owner.get(p_body_A), *bd_b = body_owner.get(p_body_B);
	if (!bd_a || !bd_b) return RID();
	JoltJointData *jd = memnew(JoltJointData(JOINT_SLIDER));
	jd->body_a = p_body_A; jd->body_b = p_body_B;
	jd->slider_frame_a = p_a; jd->slider_frame_b = p_b;
	RID rid = joint_owner.make_rid(jd);

	JoltSpaceData *sd_a = space_owner.get(bd_a->space_rid);
	if (sd_a && sd_a->system && !bd_a->body_id.IsInvalid() && !bd_b->body_id.IsInvalid()) {
		JPH::BodyLockWrite la(sd_a->system->GetBodyLockInterface(), bd_a->body_id);
		JPH::BodyLockWrite lb(sd_a->system->GetBodyLockInterface(), bd_b->body_id);
		if (la.Succeeded() && lb.Succeeded()) {
			JPH::SliderConstraintSettings s;
			s.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
			s.mPoint1 = JPH::RVec3(jv(p_a.origin));
			s.mSliderAxis1 = jv(p_a.basis.get_axis(0));
			s.mNormalAxis1 = jv(p_a.basis.get_axis(1));
			s.mPoint2 = JPH::RVec3(jv(p_b.origin));
			s.mSliderAxis2 = jv(p_b.basis.get_axis(0));
			s.mNormalAxis2 = jv(p_b.basis.get_axis(1));
			jd->constraint = static_cast<JPH::TwoBodyConstraint *>(s.Create(la.GetBody(), lb.GetBody()));
			if (jd->constraint) sd_a->system->AddConstraint(jd->constraint);
		}
	}
	return rid;
}

void JoltPhysicsServer::slider_joint_set_param(RID p_j, SliderJointParam p, float v) {
	JoltJointData *jd = joint_owner.get(p_j); if (!jd) return;
	switch (p) {
		case SLIDER_JOINT_LINEAR_LIMIT_UPPER: jd->slider_linear_limit_upper = v; break;
		case SLIDER_JOINT_LINEAR_LIMIT_LOWER: jd->slider_linear_limit_lower = v; break;
		case SLIDER_JOINT_ANGULAR_LIMIT_UPPER: jd->slider_angular_limit_upper = v; break;
		case SLIDER_JOINT_ANGULAR_LIMIT_LOWER: jd->slider_angular_limit_lower = v; break;
		default: break;
	}
}
float JoltPhysicsServer::slider_joint_get_param(RID p_j, SliderJointParam p) const {
	const JoltJointData *jd = joint_owner.get(p_j); if (!jd) return 0.0;
	switch (p) {
		case SLIDER_JOINT_LINEAR_LIMIT_UPPER: return jd->slider_linear_limit_upper;
		case SLIDER_JOINT_LINEAR_LIMIT_LOWER: return jd->slider_linear_limit_lower;
		case SLIDER_JOINT_ANGULAR_LIMIT_UPPER: return jd->slider_angular_limit_upper;
		case SLIDER_JOINT_ANGULAR_LIMIT_LOWER: return jd->slider_angular_limit_lower;
		default: return 0.0;
	}
}

RID JoltPhysicsServer::joint_create_cone_twist(RID p_body_A, const Transform &p_a, RID p_body_B, const Transform &p_b) {
	JoltBodyData *bd_a = body_owner.get(p_body_A), *bd_b = body_owner.get(p_body_B);
	if (!bd_a || !bd_b) return RID();
	JoltJointData *jd = memnew(JoltJointData(JOINT_CONE_TWIST));
	jd->body_a = p_body_A; jd->body_b = p_body_B;
	jd->conetwist_frame_a = p_a; jd->conetwist_frame_b = p_b;
	RID rid = joint_owner.make_rid(jd);

	JoltSpaceData *sd_a = space_owner.get(bd_a->space_rid);
	if (sd_a && sd_a->system && !bd_a->body_id.IsInvalid() && !bd_b->body_id.IsInvalid()) {
		JPH::BodyLockWrite la(sd_a->system->GetBodyLockInterface(), bd_a->body_id);
		JPH::BodyLockWrite lb(sd_a->system->GetBodyLockInterface(), bd_b->body_id);
		if (la.Succeeded() && lb.Succeeded()) {
			JPH::ConeConstraintSettings s;
			s.mPoint1 = JPH::RVec3(jv(p_a.origin));
			s.mTwistAxis1 = jv(p_a.basis.get_axis(1));
			s.mPoint2 = JPH::RVec3(jv(p_b.origin));
			s.mTwistAxis2 = jv(p_b.basis.get_axis(1));
			s.mHalfConeAngle = jd->conetwist_swing_span * 0.5f;
			jd->constraint = static_cast<JPH::TwoBodyConstraint *>(s.Create(la.GetBody(), lb.GetBody()));
			if (jd->constraint) sd_a->system->AddConstraint(jd->constraint);
		}
	}
	return rid;
}

void JoltPhysicsServer::cone_twist_joint_set_param(RID p_j, ConeTwistJointParam p, float v) {
	JoltJointData *jd = joint_owner.get(p_j); if (!jd) return;
	switch (p) {
		case CONE_TWIST_JOINT_SWING_SPAN: jd->conetwist_swing_span = v; break;
		case CONE_TWIST_JOINT_TWIST_SPAN: jd->conetwist_twist_span = v; break;
		case CONE_TWIST_JOINT_BIAS: jd->conetwist_bias = v; break;
		case CONE_TWIST_JOINT_SOFTNESS: jd->conetwist_softness = v; break;
		case CONE_TWIST_JOINT_RELAXATION: jd->conetwist_relaxation = v; break;
		case CONE_TWIST_MAX: break;
	}
}
float JoltPhysicsServer::cone_twist_joint_get_param(RID p_j, ConeTwistJointParam p) const {
	const JoltJointData *jd = joint_owner.get(p_j); if (!jd) return 0.0;
	if (p == CONE_TWIST_JOINT_SWING_SPAN) return jd->conetwist_swing_span;
	if (p == CONE_TWIST_JOINT_TWIST_SPAN) return jd->conetwist_twist_span;
	if (p == CONE_TWIST_JOINT_BIAS) return jd->conetwist_bias;
	if (p == CONE_TWIST_JOINT_SOFTNESS) return jd->conetwist_softness;
	if (p == CONE_TWIST_JOINT_RELAXATION) return jd->conetwist_relaxation;
	return 0.0;
}

RID JoltPhysicsServer::joint_create_generic_6dof(RID p_body_A, const Transform &p_a, RID p_body_B, const Transform &p_b) {
	JoltBodyData *bd_a = body_owner.get(p_body_A), *bd_b = body_owner.get(p_body_B);
	if (!bd_a || !bd_b) return RID();
	JoltJointData *jd = memnew(JoltJointData(JOINT_6DOF));
	jd->body_a = p_body_A; jd->body_b = p_body_B;
	jd->dof_frame_a = p_a; jd->dof_frame_b = p_b;
	RID rid = joint_owner.make_rid(jd);

	JoltSpaceData *sd_a = space_owner.get(bd_a->space_rid);
	if (sd_a && sd_a->system && !bd_a->body_id.IsInvalid() && !bd_b->body_id.IsInvalid()) {
		JPH::BodyLockWrite la(sd_a->system->GetBodyLockInterface(), bd_a->body_id);
		JPH::BodyLockWrite lb(sd_a->system->GetBodyLockInterface(), bd_b->body_id);
		if (la.Succeeded() && lb.Succeeded()) {
			JPH::SixDOFConstraintSettings s;
			s.mPosition1 = JPH::RVec3(jv(p_a.origin));
			s.mAxisX1 = jv(p_a.basis.get_axis(0));
			s.mAxisY1 = jv(p_a.basis.get_axis(1));
			s.mPosition2 = JPH::RVec3(jv(p_b.origin));
			s.mAxisX2 = jv(p_b.basis.get_axis(0));
			s.mAxisY2 = jv(p_b.basis.get_axis(1));
			for (int i = 0; i < 6; i++) s.MakeFreeAxis((JPH::SixDOFConstraintSettings::EAxis)i);
			jd->constraint = static_cast<JPH::TwoBodyConstraint *>(s.Create(la.GetBody(), lb.GetBody()));
			if (jd->constraint) sd_a->system->AddConstraint(jd->constraint);
		}
	}
	return rid;
}

void JoltPhysicsServer::generic_6dof_joint_set_param(RID, Vector3::Axis, G6DOFJointAxisParam, float) {}
float JoltPhysicsServer::generic_6dof_joint_get_param(RID, Vector3::Axis, G6DOFJointAxisParam) { return 0.0; }
void JoltPhysicsServer::generic_6dof_joint_set_flag(RID, Vector3::Axis, G6DOFJointAxisFlag, bool) {}
bool JoltPhysicsServer::generic_6dof_joint_get_flag(RID, Vector3::Axis, G6DOFJointAxisFlag) { return false; }

// ========== LIFECYCLE ==========
void JoltPhysicsServer::free(RID p_rid) {
	if (shape_owner.owns(p_rid)) { shape_owner.free(p_rid); return; }
	if (space_owner.owns(p_rid)) { space_owner.free(p_rid); return; }
	if (body_owner.owns(p_rid)) {
		JoltBodyData *bd = body_owner.get(p_rid);
		if (bd) {
			JoltSpaceData *sd = space_owner.get(bd->space_rid);
			if (sd && sd->system && !bd->body_id.IsInvalid()) {
				sd->system->GetBodyInterface().RemoveBody(bd->body_id);
				sd->system->GetBodyInterface().DestroyBody(bd->body_id);
			}
			if (bd->direct_state) memdelete(bd->direct_state);
		}
		body_owner.free(p_rid);
		return;
	}
	if (area_owner.owns(p_rid)) { area_owner.free(p_rid); return; }
	if (joint_owner.owns(p_rid)) {
		JoltJointData *jd = joint_owner.get(p_rid);
		if (jd && jd->constraint) {
			jd->constraint = nullptr;
		}
		joint_owner.free(p_rid);
		return;
	}
	if (soft_body_owner.owns(p_rid)) {
		JoltBodyData *bd = soft_body_owner.get(p_rid);
		if (bd && bd->direct_state) memdelete(bd->direct_state);
		soft_body_owner.free(p_rid);
		return;
	}
}

void JoltPhysicsServer::set_active(bool) {}
void JoltPhysicsServer::init() {}

void JoltPhysicsServer::step(float p_step) {
	List<RID> ids;
	space_owner.get_owned_list(&ids);
	for (List<RID>::Element *E = ids.front(); E; E = E->next()) {
		JoltSpaceData *sd = space_owner.get(E->get());
		if (sd) sd->step(p_step, this);
	}
}

void JoltPhysicsServer::flush_queries() { flushing_queries_flag = true; flushing_queries_flag = false; }

void JoltPhysicsServer::finish() {
	List<RID> owned;
	body_owner.get_owned_list(&owned); for (List<RID>::Element *E = owned.front(); E; E = E->next()) free(E->get());
	area_owner.get_owned_list(&owned); for (List<RID>::Element *E = owned.front(); E; E = E->next()) free(E->get());
	joint_owner.get_owned_list(&owned); for (List<RID>::Element *E = owned.front(); E; E = E->next()) free(E->get());
	space_owner.get_owned_list(&owned); for (List<RID>::Element *E = owned.front(); E; E = E->next()) free(E->get());
	shape_owner.get_owned_list(&owned); for (List<RID>::Element *E = owned.front(); E; E = E->next()) free(E->get());
	soft_body_owner.get_owned_list(&owned); for (List<RID>::Element *E = owned.front(); E; E = E->next()) free(E->get());
}

bool JoltPhysicsServer::is_flushing_queries() const { return flushing_queries_flag; }
void JoltPhysicsServer::set_collision_iterations(int i) { collision_iterations = i; }
int JoltPhysicsServer::get_process_info(ProcessInfo) { return 0; }
