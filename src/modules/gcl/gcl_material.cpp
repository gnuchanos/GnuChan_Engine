/**************************************************************************/
/*  gcl_material.cpp                                                      */
/**************************************************************************/
/*  GCL Material API (gcl.md @extern Mesh -> REF.Material.*).             */
/*                                                                        */
/*  Maps GCL names onto Godot 3 SpatialMaterial string properties via     */
/*  Object::get/set so no SpatialMaterial header is needed.               */
/**************************************************************************/

#include "gcl_material.h"

#include "core/array.h"
#include "core/class_db.h"
#include "core/color.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/object.h"
#include "core/reference.h"
#include "core/resource.h"
#include "core/vector.h"
#include "gcl_nodes.h"
#include "gcl_script.h"
#include "scene/main/node.h"

static const char *kMatProps[] = {
	"albedo_color",
	"albedo_texture",
	"metallic",
	"metallic_specular",
	"roughness",
	"emission",
	"emission_energy",
	"emission_color",
	"emission_texture",
	"normal_enabled",
	"normal_scale",
	"normal_texture",
	"ao_light_affect",
	"alpha",
	"transparency",
	"shadows",
	"cull_mode",
	"blend_mode",
	"shading_mode",
	"uv1_offset",
	"uv1_scale",
	"detail_albedo",
	"detail_roughness",
	"detail_normal",
	nullptr
};

/* GCL name -> SpatialMaterial property name. */
static String gcl_mat_prop(const String &p_seg) {
	if (p_seg == "Color") {
		return "albedo_color";
	}
	if (p_seg == "Albedo") {
		return "albedo_texture";
	}
	if (p_seg == "Metallic") {
		return "metallic";
	}
	if (p_seg == "Specular") {
		return "metallic_specular";
	}
	if (p_seg == "Roughness") {
		return "roughness";
	}
	if (p_seg == "Emission") {
		return "emission";
	}
	if (p_seg == "EmissionColor") {
		return "emission_color";
	}
	if (p_seg == "EmissionTexture") {
		return "emission_texture";
	}
	if (p_seg == "NormalMap.Enable") {
		return "normal_enabled";
	}
	if (p_seg == "NormalMap.Scale") {
		return "normal_scale";
	}
	if (p_seg == "NormalMap.Texture") {
		return "normal_texture";
	}
	if (p_seg == "AO") {
		return "ao_light_affect";
	}
	if (p_seg == "Opacity") {
		return "alpha";
	}
	if (p_seg == "Transparent") {
		return "transparency";
	}
	if (p_seg == "CastShadows") {
		return "shadows";
	}
	if (p_seg == "BlendMode") {
		return "blend_mode";
	}
	if (p_seg == "ShadingMode") {
		return "shading_mode";
	}
	if (p_seg == "UV1Offset") {
		return "uv1_offset";
	}
	if (p_seg == "UV1Scale") {
		return "uv1_scale";
	}
	if (p_seg == "Detail.Albedo") {
		return "detail_albedo";
	}
	if (p_seg == "Detail.Roughness") {
		return "detail_roughness";
	}
	if (p_seg == "Detail.Normal") {
		return "detail_normal";
	}
	return "";
}

/* Texture or file name -> Resource, loading via ResourceLoader when the
 * value is a "res://" path. */
static Variant gcl_to_texture(const Variant &p_v) {
	if (p_v.get_type() == Variant::STRING) {
		String s = p_v;
		if (s.begins_with("res://") || s.begins_with("user://")) {
			return ResourceLoader::load(s);
		}
	}
	return p_v;
}

/* Copies every known material property from p_src to p_dst (Godot 3 has no
 * copy_from on Material in all versions; manual property copy avoids
 * "Invalid call" spam). */
static void copy_material_props(Object *p_src, Object *p_dst) {
	if (!p_src || !p_dst) {
		return;
	}
	const char **p = kMatProps;
	while (*p) {
		String prop = *p;
		bool valid = false;
		Variant v = p_src->get(prop, &valid);
		if (valid) {
			p_dst->set(prop, v);
		}
		p++;
	}
}

bool gcl_material_for_node(Node *p_node, Object *&r_material, String *r_error, int p_line) {
	if (!p_node) {
		if (r_error) {
			*r_error = "Material: null node (line " + itos(p_line) + ")";
		}
		return false;
	}
	Variant override_mat = p_node->get("material_override");
	Object *mat = override_mat.get_type() == Variant::OBJECT ? (Object *)override_mat : nullptr;
	if (!mat) {
		String class_name = "SpatialMaterial";
		Object *created = ClassDB::instance(class_name);
		if (!created) {
			if (r_error) {
				*r_error = "Material: cannot create " + class_name + " (line " + itos(p_line) + ")";
			}
			return false;
		}
		p_node->set("material_override", created);
		mat = created;
	}
	r_material = mat;
	return true;
}

bool gcl_material_segment(Object *p_material, const String &p_seg, Variant &r_out) {
	if (!p_material) {
		return false;
	}
	String prop = gcl_mat_prop(p_seg);
	if (prop == "") {
		return false;
	}
	if (p_seg == "Emission") {
		r_out = p_material->get("emission_energy");
		return true;
	}
	if (p_seg == "Metallic") {
		r_out = p_material->get("metallic");
		return true;
	}
	if (p_seg == "Roughness") {
		r_out = p_material->get("roughness");
		return true;
	}
	if (p_seg == "Opacity") {
		r_out = p_material->get("alpha");
		return true;
	}
	if (p_seg == "CastShadows") {
		r_out = p_material->get("shadows");
		return true;
	}
	if (p_seg == "BlendMode") {
		r_out = p_material->get("blend_mode");
		return true;
	}
	if (p_seg == "ShadingMode") {
		r_out = p_material->get("shading_mode");
		return true;
	}
	if (p_seg == "UV1Offset") {
		r_out = p_material->get("uv1_offset");
		return true;
	}
	if (p_seg == "UV1Scale") {
		r_out = p_material->get("uv1_scale");
		return true;
	}
	r_out = p_material->get(prop);
	return true;
}

bool gcl_material_write(Object *p_material, const String &p_seg, const Variant &p_value) {
	if (!p_material) {
		return false;
	}
	String prop = gcl_mat_prop(p_seg);
	if (prop == "") {
		return false;
	}
	if (p_seg == "Emission") {
		p_material->set("emission_energy", p_value);
		return true;
	}
	Variant val = p_value;
	if (prop.find("texture") != -1) {
		val = gcl_to_texture(val);
	}
	p_material->set(prop, val);
	return true;
}

/* Resolves "domain.sub" pairs (or single segments when p_sub is empty). */
static String gcl_mat_pair_prop(const String &p_seg, const String &p_sub) {
	String full = p_seg;
	if (p_sub != "") {
		full += "." + p_sub;
	}
	return gcl_mat_prop(full);
}

static bool is_texture_prop(const String &p_seg, const String &p_sub) {
	if (p_sub == "Texture" || p_sub == "Albedo" || p_sub == "Roughness" || p_sub == "Normal") {
		return p_seg == "Metallic" || p_seg == "Roughness" || p_seg == "Emission" || p_seg == "AO" || p_seg == "NormalMap" || p_seg == "Detail";
	}
	return false;
}

bool gcl_material_resolve(Object *p_material, const String &p_seg, const String &p_sub, Variant &r_out) {
	if (!p_material) {
		return false;
	}
	if (p_seg == "Emission") {
		if (p_sub == "") {
			Variant tmp;
			r_out = gcl_material_segment(p_material, "Emission", tmp) ? tmp : Variant();
			return true;
		}
		if (p_sub == "Color") {
			r_out = p_material->get("emission_color");
			return true;
		}
		if (p_sub == "Texture") {
			r_out = p_material->get("emission_texture");
			return true;
		}
		return false;
	}
	if (p_seg == "AO" && p_sub == "Texture") {
		r_out = p_material->get(p_sub == "Texture" ? "ao_texture" : "ao_light_affect");
		return true;
	}
	if (p_sub != "") {
		String prop = gcl_mat_pair_prop(p_seg, p_sub);
		if (prop != "") {
			r_out = p_material->get(prop);
			return true;
		}
		return false;
	}
	return gcl_material_segment(p_material, p_seg, r_out);
}

bool gcl_material_resolve_write(Object *p_material, const String &p_seg, const String &p_sub, const Variant &p_value) {
	if (!p_material) {
		return false;
	}
	if (p_seg == "Emission") {
		if (p_sub == "") {
			p_material->set("emission_energy", p_value);
			return true;
		}
		if (p_sub == "Color") {
			p_material->set("emission_color", p_value);
			return true;
		}
		if (p_sub == "Texture") {
			p_material->set("emission_texture", gcl_to_texture(p_value));
			return true;
		}
		return false;
	}
	if (p_seg == "AO" && p_sub == "Texture") {
		p_material->set("ao_texture", gcl_to_texture(p_value));
		return true;
	}
	if (p_sub != "") {
		String prop = gcl_mat_pair_prop(p_seg, p_sub);
		if (prop != "") {
			Variant val = is_texture_prop(p_seg, p_sub) ? gcl_to_texture(p_value) : p_value;
			p_material->set(prop, val);
			return true;
		}
		return false;
	}
	return gcl_material_write(p_material, p_seg, p_value);
}

bool gcl_material_method(Object *p_material, const String &p_mname, const Array &p_args, Variant &r_out, String *r_error, int p_line) {
	if (!p_material) {
		return false;
	}
	if (p_mname == "Clear") {
		p_material->call("clear");
		r_out = Variant();
		return true;
	}
	if (p_mname == "Copy") {
		if (p_args.size() > 0 && p_args[0].get_type() == Variant::OBJECT) {
			copy_material_props(p_args[0], p_material);
			r_out = Variant();
			return true;
		}
		return false;
	}
	if (p_mname == "Save") {
		if (p_args.size() > 0 && p_args[0].get_type() == Variant::STRING) {
			Resource *res = Object::cast_to<Resource>(p_material);
			if (res) {
				ResourceSaver::save(p_args[0], Ref<Resource>(res));
			}
			r_out = Variant();
			return true;
		}
		return false;
	}
	if (p_mname == "Load") {
		if (p_args.size() > 0 && p_args[0].get_type() == Variant::STRING) {
			Ref<Resource> loaded = ResourceLoader::load(p_args[0]);
			if (loaded.is_valid()) {
				copy_material_props(loaded.ptr(), p_material);
			}
			r_out = Variant();
			return true;
		}
		return false;
	}
	return false;
}
