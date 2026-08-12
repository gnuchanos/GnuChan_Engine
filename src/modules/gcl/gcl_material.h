/**************************************************************************/
/*  gcl_material.h                                                        */
/**************************************************************************/
/*  GCL Material API (gcl.md @extern Mesh -> REF.Material.*).             */
/*                                                                        */
/*  Maps GCL material names to Godot 3 SpatialMaterial properties:        */
/*    Color, Albedo, Metallic(+Specular/Texture), Roughness(+Texture),    */
/*    Emission(+Color/Texture), NormalMap(Enable/Scale/Texture), AO,      */
/*    Opacity, Transparent, CastShadows, DoubleSided, CullBackface,       */
/*    BlendMode, ShadingMode, UV1Offset, UV1Scale, Detail(Albedo/Rough..) */
/*    Clear(), Copy(other), Save(path), Load(path)                        */
/**************************************************************************/

#ifndef GCL_MATERIAL_H
#define GCL_MATERIAL_H

#include "gcl_core.h"

class Node;
class Object;

/* Lazily returns the material_override of a mesh node, creating a fresh
 * SpatialMaterial if none is set. Returns false on failure. */
bool gcl_material_for_node(Node *p_node, Object *&r_material, String *r_error, int p_line);

/* Read hook. Returns true when p_seg is a GCL material property. */
bool gcl_material_segment(Object *p_material, const String &p_seg, Variant &r_out);

/* Write hook. Returns true when p_seg is a GCL material property. */
bool gcl_material_write(Object *p_material, const String &p_seg, const Variant &p_value);

/* Two-segment resolve for domain+sub pairs from gcl.md:
 *   Metallic.Specular, Metallic.Texture, Roughness.Texture,
 *   Emission.Color, Emission.Texture, NormalMap.Enable/Scale/Texture,
 *   AO.Texture, Detail.Albedo/Roughness/Normal
 * p_sub may be empty (plain single-segment read). Returns true when the
 * property is known and r_out holds its value. */
bool gcl_material_resolve(Object *p_material, const String &p_seg, const String &p_sub, Variant &r_out);

/* Write equivalent of gcl_material_resolve. */
bool gcl_material_resolve_write(Object *p_material, const String &p_seg, const String &p_sub, const Variant &p_value);

/* Method hook: Clear(), Copy(...), Save(...), Load(...). */
bool gcl_material_method(Object *p_material, const String &p_mname, const Array &p_args, Variant &r_out, String *r_error, int p_line);

#endif // GCL_MATERIAL_H
