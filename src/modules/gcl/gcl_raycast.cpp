/*************************************************************************/
/*  gcl_raycast.cpp                                                       */
/*************************************************************************/
/*  GCL Raycast API (gcl.md @extern Raycast).                             */
/*                                                                        */
/*  Uses generic Object::call() so no RayCast header is needed:           */
/*    is_colliding(), get_collider(), add_exception(object), cast_to.     */
/*************************************************************************/

#include "gcl_raycast.h"

#include "gcl_nodes.h"
#include "gcl_script.h"

#include "core/array.h"
#include "core/math/vector3.h"
#include "core/vector.h"
#include "scene/main/node.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

bool gcl_raycast_segment(Node *p_node, const String &p_seg, Variant &r_out, String *r_error, int p_line) {
	if (!p_node || !p_node->is_class("RayCast")) {
		return false;
	}
	if (p_seg == "IsColliding") {
		r_out = Variant((bool)p_node->call("is_colliding"));
		return true;
	}
	if (p_seg == "GetBodyName") {
		Variant collider = p_node->call("get_collider");
		if (collider.get_type() == Variant::OBJECT && collider.operator Object *()) {
			Node *cn = Object::cast_to<Node>(collider.operator Object *());
			if (cn) {
				r_out = Variant(cn->get_name().operator String());
			} else {
				r_out = Variant(String());
			}
		} else {
			r_out = Variant(String());
		}
		return true;
	}
	if (p_seg == "length") {
		Variant cast_to = p_node->get("cast_to");
		if (cast_to.get_type() == Variant::VECTOR3) {
			Vector3 vec = cast_to;
			r_out = Variant(vec.length());
		} else {
			r_out = Variant(0.0f);
		}
		return true;
	}
	return false;
}

bool gcl_raycast_method(Node *p_node, const String &p_mname, const Array &p_args, Variant &r_out, String *r_error, int p_line) {
	if (!p_node || !p_node->is_class("RayCast")) {
		return false;
	}
	if (p_mname == "Skip") {
		if (p_args.size() > 0) {
			Variant a = p_args[0];
			if (a.get_type() == Variant::OBJECT && a.operator Object *()) {
				Object *target = a.operator Object *();
				p_node->call("add_exception", target);
				r_out = Variant();
				return true;
			}
			if (a.get_type() == Variant::STRING) {
				String name = a;
				// resolve by name against the whole tree (root is a Viewport
				// in this fork, not a Node - cast it; fall back to p_node)
				Node *root = p_node->is_inside_tree() ? Object::cast_to<Node>(p_node->get_tree()->get_root()) : nullptr;
				Node *found = root ? gcl_find_node_ci(root, name) : nullptr;
				if (!found) {
					found = gcl_find_node_ci(p_node, name);
				}
				if (found) {
					p_node->call("add_exception", found);
					r_out = Variant();
					return true;
				}
				if (r_error && *r_error == "") {
					*r_error = "Skip: node '" + name + "' not found (line " + itos(p_line) + ")";
				}
				r_out = Variant();
				return true;
			}
		}
		return false;
	}
	if (p_mname == "SkipList") {
		if (p_args.size() > 0 && p_args[0].get_type() == Variant::ARRAY) {
			Array names = p_args[0];
			Node *root = p_node->is_inside_tree() ? Object::cast_to<Node>(p_node->get_tree()->get_root()) : nullptr;
			for (int i = 0; i < names.size(); i++) {
				if (names[i].get_type() == Variant::STRING) {
					Node *found = root ? gcl_find_node_ci(root, (String)names[i]) : nullptr;
					if (found) {
						p_node->call("add_exception", found);
					}
				}
			}
			r_out = Variant();
			return true;
		}
		return false;
	}
	return false;
}

bool gcl_raycast_write(Node *p_node, const String &p_seg, const Variant &p_value) {
	if (!p_node || p_seg != "length") {
		return false;
	}
	Variant cast_to = p_node->get("cast_to");
	if (cast_to.get_type() == Variant::VECTOR3) {
		Vector3 vec = cast_to;
		float len = (float)p_value;
		if (vec.length() > 0.0001f) {
			vec = vec.normalized() * len;
		} else {
			vec = Vector3(0, 0, -1) * len;
		}
		p_node->set("cast_to", vec);
		return true;
	}
	return false;
}
