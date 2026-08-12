/**************************************************************************/
/*  gcl_nodes.cpp                                                         */
/**************************************************************************/
/*  GCL NODE SYSTEM implementation.                                       */
/*                                                                        */
/*  Covers: GetNode("name") global, Name/Position/Rotation/Scale/GetChild/ */
/*  Childs/Hide/Show/Free/Enable/Disable segments, Find("name") method,   */
/*  and $path/FindNode/NodePath resolution.                               */
/**************************************************************************/

#include "gcl_nodes.h"

#include "gcl_script.h"

#include "scene/3d/spatial.h"
#include "scene/main/node.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

/* -------------------------------------------------------------------- */
/*  Resolution                                                           */
/* -------------------------------------------------------------------- */

Node *gcl_find_node_ci(Node *p_from, const String &p_name) {
	if (!p_from) {
		return nullptr;
	}
	if (p_from->get_name().operator String().casecmp_to(p_name) == 0) {
		return p_from;
	}
	for (int i = 0; i < p_from->get_child_count(); i++) {
		Node *hit = gcl_find_node_ci(p_from->get_child(i), p_name);
		if (hit) {
			return hit;
		}
	}
	return nullptr;
}

/* Whether any node in the whole tree matches the name (case-insensitive). */
static Node *gcl_tree_find(GCLScriptInstance *p_inst, const String &p_name, String *r_error, int p_line) {
	Object *owner = p_inst->get_owner();
	if (!owner) {
		if (r_error) {
			*r_error = "GetNode: no owner node (line " + itos(p_line) + ")";
		}
		return nullptr;
	}
	Node *owner_node = Object::cast_to<Node>(owner);
	if (!owner_node || !owner_node->is_inside_tree()) {
		if (r_error) {
			*r_error = "GetNode: owner is not inside the tree (line " + itos(p_line) + ")";
		}
		return nullptr;
	}
	Node *found = gcl_find_node_ci(Object::cast_to<Node>(owner_node->get_tree()->get_root()), p_name);
	if (!found) {
		if (r_error) {
			*r_error = "GetNode: '" + p_name + "' not found (line " + itos(p_line) + ")";
		}
		return nullptr;
	}
	return found;
}

Variant gcl_node_by_name(GCLScriptInstance *p_inst, const String &p_name, String *r_error, int p_line) {
	Node *n = gcl_tree_find(p_inst, p_name, r_error, p_line);
	return n ? Variant(n) : Variant();
}

Variant gcl_find_node(GCLScriptInstance *p_inst, const String &p_name, String *r_error, int p_line) {
	return gcl_node_by_name(p_inst, p_name, r_error, p_line);
}

Variant gcl_resolve_path(GCLScriptInstance *p_inst, const String &p_path, String *r_error, int p_line) {
	Object *owner = p_inst->get_owner();
	Node *owner_node = Object::cast_to<Node>(owner);
	if (!owner_node || !owner_node->is_inside_tree()) {
		if (r_error) {
			*r_error = "$path: owner is not a node inside the tree (line " + itos(p_line) + ")";
		}
		return Variant();
	}
	Node *root = Object::cast_to<Node>(owner_node->get_tree()->get_root());
	if (p_path.begins_with("/")) {
		Node *n = root->get_node_or_null(p_path);
		if (n) {
			return Variant(n);
		}
	} else {
		Node *n = owner_node->get_node_or_null(p_path);
		if (n) {
			return Variant(n);
		}
		n = root->get_node_or_null(p_path);
		if (n) {
			return Variant(n);
		}
		n = root->get_node_or_null("/" + p_path);
		if (n) {
			return Variant(n);
		}
	}
	int slash = p_path.rfind("/");
	String basename = slash >= 0 ? p_path.substr(slash + 1) : p_path;
	if (basename != "") {
		Node *found = gcl_find_node_ci(root, basename);
		if (found) {
			return Variant(found);
		}
	}
	if (r_error) {
		*r_error = "$path: '" + p_path + "' not found (line " + itos(p_line) + ")";
	}
	return Variant();
}

Variant gcl_variant_resolve_node(GCLScriptInstance *p_inst, const Variant &p_v) {
	if (p_v.get_type() == Variant::NODE_PATH) {
		NodePath np = p_v;
		Object *owner = p_inst->get_owner();
		Node *owner_node = Object::cast_to<Node>(owner);
		if (owner_node && owner_node->is_inside_tree()) {
			Node *n = owner_node->get_node_or_null(np);
			if (!n) {
				Node *root = Object::cast_to<Node>(owner_node->get_tree()->get_root());
				if (root) {
					n = root->get_node_or_null(np);
				}
			}
			if (n) {
				return Variant(n);
			}
		}
		return Variant();
	}
	return p_v;
}

/* -------------------------------------------------------------------- */
/*  GCL property aliases                                                 */
/* -------------------------------------------------------------------- */

String gcl_node_property_alias(const String &p_seg) {
	if (p_seg == "Name") {
		return "name";
	}
	if (p_seg == "Position") {
		return "translation";
	}
	if (p_seg == "Rotation") {
		return "rotation_degrees";
	}
	if (p_seg == "Scale") {
		return "scale";
	}
	return "";
}

/* -------------------------------------------------------------------- */
/*  Segments: Name, Position, Rotation, Scale, GetChild, Childs, and     */
/*  actions Hide/Show/Free/Enable/Disable                                */
/* -------------------------------------------------------------------- */

bool gcl_node_segment(Node *p_node, const String &p_seg, Variant &r_out, String *r_error, int p_line) {
	if (!p_node) {
		return false;
	}
	if (p_seg == "Childs") {
		Array children;
		int child_count = p_node->get_child_count();
		for (int ci = 0; ci < child_count; ci++) {
			children.push_back(Variant(p_node->get_child(ci)));
		}
		r_out = children;
		return true;
	}
	if (p_seg == "GetChild") {
		// GetChild is a pseudo-read returning a directive when followed by
		// Find("name"). We model it as: return the node itself; the next
		// segment (Find) operates on it and searches its children.
		r_out = Variant(p_node);
		return true;
	}
	if (p_seg == "GetNode") {
		// GetNode (no parens) returns the node itself so chains keep going:
		//   NODE REF = self.Raycast.GetNode;   -> REF is the Raycast node
		//   ABC.GetNode.Name                    -> child body name
		r_out = Variant(p_node);
		return true;
	}
	if (p_seg == "Hide") {
		// Not call("hide"): hide() only exists on CanvasItem and would emit
		// "Invalid call" on Spatial. visible works for anything that has it
		// and fails silently otherwise.
		p_node->set("visible", false);
		r_out = Variant();
		return true;
	}
	if (p_seg == "Show") {
		p_node->set("visible", true);
		r_out = Variant();
		return true;
	}
	if (p_seg == "Free") {
		p_node->queue_delete();
		r_out = Variant();
		return true;
	}
	if (p_seg == "Enable") {
		p_node->set("disabled", false);
		p_node->set("monitoring", true);
		p_node->call("set_process", true);
		p_node->call("set_physics_process", true);
		r_out = Variant();
		return true;
	}
	if (p_seg == "Disable") {
		// gcl.md: "child collision devre disi kalir". Turn off the collision
		// shape/area (disabled) plus monitoring (Area) and processing.
		p_node->set("disabled", true);
		p_node->set("monitoring", false);
		p_node->call("set_process", false);
		p_node->call("set_physics_process", false);
		r_out = Variant();
		return true;
	}
	return false;
}

bool gcl_node_method(Node *p_node, const String &p_mname, const Array &p_args, Variant &r_out, String *r_error, int p_line) {
	if (!p_node) {
		return false;
	}
	if (p_mname == "Find") {
		if (p_args.size() > 0 && p_args[0].get_type() == Variant::STRING) {
			String name = p_args[0];
			for (int ci = 0; ci < p_node->get_child_count(); ci++) {
				Node *child = p_node->get_child(ci);
				if (child && child->get_name().operator String().casecmp_to(name) == 0) {
					r_out = Variant(child);
					return true;
				}
			}
			r_out = Variant();
			if (r_error && *r_error == "") {
				*r_error = "Find: child '" + name + "' not found (line " + itos(p_line) + ")";
			}
			return true;
		}
	}
	return false;
}
