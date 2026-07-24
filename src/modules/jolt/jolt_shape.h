#ifndef JOLT_SHAPE_H
#define JOLT_SHAPE_H

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Core/Reference.h>
#include "core/rid.h"
#include "servers/physics_server.h"

class JoltShapeData : public RID_Data {
public:
	PhysicsServer::ShapeType type;
	JPH::RefConst<JPH::Shape> shape;
	real_t margin = 0.0;
	real_t custom_solver_bias = 0.0;
	Variant data;

	JoltShapeData(PhysicsServer::ShapeType p_type) : type(p_type) {}
	~JoltShapeData() override = default;
};

#endif
