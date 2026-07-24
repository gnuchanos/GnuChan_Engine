#include "register_types.h"
#include "core/class_db.h"
#include "core/project_settings.h"
#include "jolt_physics_server.h"

void register_jolt_types() {
	// Don't register JoltPhysicsServer in ClassDB. PhysicsServer virtual class
	// is already registered by register_server_types(), and registering a child
	// class triggers GDCLASS's initialize_class() which re-binds parent 
	// methods/constants (including BIND_ENUM_CONSTANT calls in PhysicsServer::_bind_methods),
	// causing "constant already exists" crash at ClassDB::bind_integer_constant.

	// Only register the server factory and settings
	PhysicsServerManager::register_server("JoltPhysics", &JoltPhysicsServer::create_callback);
	PhysicsServerManager::set_default_server("JoltPhysics", 100);

	// Jolt-specific project settings
	GLOBAL_DEF("physics/3d/jolt_physics/max_bodies", 65536);
	ProjectSettings::get_singleton()->set_custom_property_info(
		"physics/3d/jolt_physics/max_bodies",
		PropertyInfo(Variant::INT, "physics/3d/jolt_physics/max_bodies",
			PROPERTY_HINT_RANGE, "1024,262144,1"));

	GLOBAL_DEF("physics/3d/jolt_physics/max_body_pairs", 65536);
	ProjectSettings::get_singleton()->set_custom_property_info(
		"physics/3d/jolt_physics/max_body_pairs",
		PropertyInfo(Variant::INT, "physics/3d/jolt_physics/max_body_pairs",
			PROPERTY_HINT_RANGE, "1024,262144,1"));

	GLOBAL_DEF("physics/3d/jolt_physics/max_contact_constraints", 10240);
	ProjectSettings::get_singleton()->set_custom_property_info(
		"physics/3d/jolt_physics/max_contact_constraints",
		PropertyInfo(Variant::INT, "physics/3d/jolt_physics/max_contact_constraints",
			PROPERTY_HINT_RANGE, "512,65536,1"));

	GLOBAL_DEF("physics/3d/jolt_physics/temp_allocator_size", 10);
	ProjectSettings::get_singleton()->set_custom_property_info(
		"physics/3d/jolt_physics/temp_allocator_size",
		PropertyInfo(Variant::INT, "physics/3d/jolt_physics/temp_allocator_size",
			PROPERTY_HINT_RANGE, "1,1024,1,or_greater"));

	GLOBAL_DEF("physics/3d/jolt_physics/collision_iterations", 1);
	ProjectSettings::get_singleton()->set_custom_property_info(
		"physics/3d/jolt_physics/collision_iterations",
		PropertyInfo(Variant::INT, "physics/3d/jolt_physics/collision_iterations",
			PROPERTY_HINT_RANGE, "1,8,1"));

	GLOBAL_DEF("physics/3d/jolt_physics/thread_count", 1);
	ProjectSettings::get_singleton()->set_custom_property_info(
		"physics/3d/jolt_physics/thread_count",
		PropertyInfo(Variant::INT, "physics/3d/jolt_physics/thread_count",
			PROPERTY_HINT_RANGE, "1,32,1"));

	GLOBAL_DEF("physics/3d/jolt_physics/enable_sleeping", true);
}

void unregister_jolt_types() {}
