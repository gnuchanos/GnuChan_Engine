/**************************************************************************/
/*  node_reference.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHANIDE ENGINE                          */
/*                        https://github.com/gnuchanos                    */
/**************************************************************************/
/*  NodeReference: safe reference to a Node using ObjectID. Does not own */
/*  the node and does not keep it alive.                                 */
/*                                                                        */
/*  Permission is hereby granted, free of charge, to any person obtaining */
/*  a copy of this software and associated documentation files (the       */
/*  "Software"), to deal in the Software without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,   */
/*  distribute, sublicense, and/or sell copies of the Software, and to    */
/*  permit persons to whom the Software is furnished to do so, subject to */
/*  the following conditions:                                             */
/*                                                                        */
/*  The above copyright notice and this permission notice shall be        */
/*  included in all copies or substantial portions of the Software.       */
/*                                                                        */
/*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/*  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/*  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/*  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/*  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/**************************************************************************/

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
