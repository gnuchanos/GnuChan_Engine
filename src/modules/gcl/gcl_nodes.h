/**************************************************************************/
/*  gcl_nodes.h                                                           */
/**************************************************************************/
/*  Node helpers for the GCL runtime.                                     */
/**************************************************************************/

#ifndef GCL_NODES_H
#define GCL_NODES_H

#include "gcl_core.h"

class GCLScriptInstance;

/* Recursive, case-insensitive subtree search. */
Node *gcl_find_node_ci(Node *p_from, const String &p_name);

/* Global helper: FindNode("name") - searches the whole tree by name. */
Variant gcl_find_node(GCLScriptInstance *p_inst, const String &p_name, String *r_error, int p_line);

/* Global helper: GetNode("name") - whole-tree lookup by name. */
Variant gcl_node_by_name(GCLScriptInstance *p_inst, const String &p_name, String *r_error, int p_line);

/* Godot style $"/path/Node" resolution. */
Variant gcl_resolve_path(GCLScriptInstance *p_inst, const String &p_path, String *r_error, int p_line);

/* NodePath-typed values resolve to real Nodes. */
Variant gcl_variant_resolve_node(GCLScriptInstance *p_inst, const Variant &p_v);

/* Maps GCL property names to engine ones (Name->name, Position->translation,
 * Rotation->rotation_degrees, Scale->scale). Returns "" if not an alias. */
String gcl_node_property_alias(const String &p_seg);

/* Handles GCL-only node segments (Childs/GetChild/GetNode/Hide/Show/Free/
 * Enable/Disable). Returns true when p_seg was a GCL keyword and r_out
 * holds the result (may be NIL for actions). */
bool gcl_node_segment(Node *p_node, const String &p_seg, Variant &r_out, String *r_error, int p_line);

/* Handles GCL-only node methods (Find("name")). Returns true when handled. */
bool gcl_node_method(Node *p_node, const String &p_mname, const Array &p_args, Variant &r_out, String *r_error, int p_line);

#endif // GCL_NODES_H
