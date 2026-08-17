
#include "node_reference.h"

#include "core/class_db.h"
#include "core/object_id.h"

void node_refferance::set_node(Object *p_node) {
	node_id = (p_node != nullptr) ? p_node->get_instance_id() : 0;
}

Object *node_refferance::get_node() const {
	if (node_id == 0) {
		return nullptr;
	}
	return ObjectDB::get_instance(node_id);
}

bool node_refferance::is_valid() const {
	return node_id != 0 && ObjectDB::get_instance(node_id) != nullptr;
}

void node_refferance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_node", "node"), &node_refferance::set_node);
	ClassDB::bind_method(D_METHOD("get_node"), &node_refferance::get_node);
	ClassDB::bind_method(D_METHOD("is_valid"), &node_refferance::is_valid);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "Node", PROPERTY_USAGE_SCRIPT_VARIABLE), "set_node", "get_node");
}

node_refferance::node_refferance() {
	node_id = 0;
}
