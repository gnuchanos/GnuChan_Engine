/**************************************************************************/
/*  gcl_raycast.h                                                         */
/**************************************************************************/
/*  GCL Raycast API (gcl.md @extern Raycast).                             */
/*                                                                        */
/*  self.IsColliding   -> bool  (raycast is_colliding)                    */
/*  self.GetBodyName   -> String (collider node name, "" if none)         */
/*  self.Skip(body)    -> add real Godot collision exception              */
/*  self.SkipList({..})-> exceptions for each name in the list            */
/*  self.length        -> cast_to length (read/write)                     */
/**************************************************************************/

#ifndef GCL_RAYCAST_H
#define GCL_RAYCAST_H

#include "gcl_core.h"

class Node;

/* Handles a GCL raycast-only segment: IsColliding, GetBodyName, length.
 * Returns true when p_seg was handled and r_out holds the result. */
bool gcl_raycast_segment(Node *p_node, const String &p_seg, Variant &r_out, String *r_error, int p_line);

/* Handles a GCL raycast-only method: Skip, SkipList.
 * Returns true when handled and r_out holds the result. */
bool gcl_raycast_method(Node *p_node, const String &p_mname, const Array &p_args, Variant &r_out, String *r_error, int p_line);

/* Write support: REF.length = <float>. Returns true when handled. */
bool gcl_raycast_write(Node *p_node, const String &p_seg, const Variant &p_value);

#endif // GCL_RAYCAST_H
