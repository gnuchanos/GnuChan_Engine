/**************************************************************************/
/*  gcl_expr.cpp                                                          */
/**************************************************************************/
/*  Expression evaluation: member paths (self.X, X.Y, $"p".X), literals,  */
/*  variable reads, array literals, and property/method writes.           */
/*                                                                        */
/*  Node behaviour is delegated to the node system module (gcl_nodes):    */
/*  GetNode("name"), Name/Position/Rotation/Scale, GetChild/Childs,       */
/*  Hide/Show/Free/Enable/Disable, Find("name").                          */
/**************************************************************************/

#include "gcl_core.h"
#include "gcl_script.h"

#include "core/array.h"
#include "core/engine.h"
#include "core/math/vector3.h"
#include "core/node_path.h"
#include "core/os/input.h"
#include "core/os/keyboard.h"
#include "core/os/os.h"
#include "gcl_material.h"
#include "gcl_nodes.h"
#include "gcl_raycast.h"
#include "scene/main/node.h"

/* -------------------------------------------------------------------- */
/*  Text helpers (shared)                                                */
/* -------------------------------------------------------------------- */

Vector<String> gcl_split_top_commas(const String &p_s) {
	Vector<String> out;
	String cur;
	bool in_str = false;
	bool in_char = false;
	for (int i = 0; i < p_s.length(); i++) {
		CharType c = p_s[i];
		if (c == '"') {
			in_str = !in_str;
		} else if (c == '\'') {
			in_char = !in_char;
		}
		if (c == ',' && !in_str && !in_char) {
			out.push_back(cur);
			cur = "";
		} else {
			cur += String::chr(c);
		}
	}
	out.push_back(cur);
	return out;
}

Vector<String> gcl_split_member_path(const String &p_s) {
	Vector<String> out;
	String cur;
	int depth = 0;
	bool in_str = false;
	bool in_char = false;
	for (int i = 0; i < p_s.length(); i++) {
		CharType c = p_s[i];
		if (c == '"') {
			in_str = !in_str;
		} else if (c == '\'') {
			in_char = !in_char;
		} else if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		}
		if (c == '.' && depth == 0 && !in_str && !in_char) {
			out.push_back(cur);
			cur = "";
		} else {
			cur += String::chr(c);
		}
	}
	if (cur != "") {
		out.push_back(cur);
	}
	return out;
}

String gcl_strip_inline_comment(const String &p_line) {
	bool in_str = false;
	bool in_char = false;
	for (int i = 0; i < p_line.length(); i++) {
		CharType c = p_line[i];
		if (c == '"') {
			in_str = !in_str;
		} else if (c == '\'') {
			in_char = !in_char;
		} else if (c == '#' && !in_str && !in_char) {
			return p_line.substr(0, i);
		}
	}
	return p_line;
}

/* -------------------------------------------------------------------- */
/*  Operator helpers (gcl_language.md: == != < > <= >= && || ! ? :      */
/*  + - * / %)                                                          */
/* -------------------------------------------------------------------- */

/* Index of the first top-level (paren depth 0, outside strings/chars)
 * occurrence of p_op, or -1. */
static int gcl_find_top_level(const String &p_s, const String &p_op) {
	int depth = 0;
	bool in_str = false;
	bool in_char = false;
	for (int i = 0; i < p_s.length(); i++) {
		CharType c = p_s[i];
		if (c == '"') {
			in_str = !in_str;
		} else if (c == '\'') {
			in_char = !in_char;
		} else if (!in_str && !in_char) {
			if (c == '(') {
				depth++;
			} else if (c == ')') {
				depth--;
			} else if (depth == 0 && p_s.substr(i, p_op.length()) == p_op) {
				return i;
			}
		}
	}
	return -1;
}

static bool gcl_truthy_val(const Variant &p_v) {
	if (p_v.get_type() == Variant::BOOL) {
		return (bool)p_v;
	}
	if (p_v.get_type() == Variant::INT) {
		return (int)p_v != 0;
	}
	if (p_v.get_type() == Variant::REAL) {
		return (float)p_v != 0.0f;
	}
	if (p_v.get_type() == Variant::STRING) {
		return (String)p_v != "";
	}
	if (p_v.get_type() == Variant::OBJECT) {
		Object *o = p_v;
		return o != nullptr;
	}
	return false;
}

static double gcl_to_double(const Variant &p_v) {
	if (p_v.get_type() == Variant::INT) {
		return (double)(int)p_v;
	}
	return (double)(float)p_v;
}

/* -1 / 0 / +1 : both numeric (int/real), string-string and bool-bool. */
static int gcl_compare_vals(const Variant &p_a, const Variant &p_b) {
	Variant::Type ta = p_a.get_type();
	Variant::Type tb = p_b.get_type();
	bool a_num = (ta == Variant::INT || ta == Variant::REAL);
	bool b_num = (tb == Variant::INT || tb == Variant::REAL);
	if (a_num && b_num) {
		double x = gcl_to_double(p_a);
		double y = gcl_to_double(p_b);
		return (x < y) ? -1 : (x > y) ? 1 : 0;
	}
	if (ta == Variant::STRING && tb == Variant::STRING) {
		String as = p_a;
		String bs = p_b;
		if (as == bs) {
			return 0;
		}
		return (as < bs) ? -1 : 1;
	}
	if (ta == Variant::BOOL && tb == Variant::BOOL) {
		bool x = (bool)p_a;
		bool y = (bool)p_b;
		return (x == y) ? 0 : (!x ? -1 : 1);
	}
	return 0;
}

static Variant gcl_arith(const Variant &p_a, const Variant &p_b, const String &p_op) {
	bool a_int = p_a.get_type() == Variant::INT;
	bool b_int = p_b.get_type() == Variant::INT;
	if (p_op == "+" && p_a.get_type() == Variant::STRING && p_b.get_type() == Variant::STRING) {
		return Variant((String)p_a + (String)p_b);
	}
	if (a_int && b_int) {
		int x = (int)p_a;
		int y = (int)p_b;
		if (p_op == "+") {
			return Variant(x + y);
		}
		if (p_op == "-") {
			return Variant(x - y);
		}
		if (p_op == "*") {
			return Variant(x * y);
		}
		if (p_op == "/") {
			return (y == 0) ? Variant() : Variant(x / y);
		}
		if (p_op == "%") {
			return (y == 0) ? Variant() : Variant(x % y);
		}
	}
	double x = gcl_to_double(p_a);
	double y = gcl_to_double(p_b);
	if (p_op == "+") {
		return Variant(x + y);
	}
	if (p_op == "-") {
		return Variant(x - y);
	}
	if (p_op == "*") {
		return Variant(x * y);
	}
	if (p_op == "/") {
		return (y == 0.0) ? Variant() : Variant(x / y);
	}
	if (p_op == "%") {
		return (y == 0.0) ? Variant() : Variant((int)x % (int)y);
	}
	return Variant();
}
/* -------------------------------------------------------------------- */
/*  Member path evaluation                                               */
/* -------------------------------------------------------------------- */

/* Evaluates the Engine domain (gcl.md "temel komutlar"). Engine is not an
 * Object in this engine fork, so it is handled entirely here, before any
 * Variant/Object conversion. */
static Variant eval_engine_path(const Vector<String> &p_segments, int p_seg_idx, const Map<StringName, Variant> &p_locals, GCLScriptInstance *p_inst, int p_line, String *r_error) {
	for (int s = p_seg_idx; s < p_segments.size(); s++) {
		String seg = p_segments[s].strip_edges();
		if (seg == "") {
			continue;
		}
		int open = seg.find("(");
		if (open != -1 && seg.ends_with(")")) {
			String mname = seg.substr(0, open).strip_edges();
			if (mname == "SetFPS") {
				// Engine.SetFPS(60)
				String inside = seg.substr(open + 1, seg.length() - open - 2);
				Vector<String> parts = gcl_split_top_commas(inside);
				if (parts.size() > 0 && parts[0].strip_edges() != "") {
					Variant fps = gcl_eval_expr(parts[0], p_locals, p_inst, p_line, r_error);
					Engine::get_singleton()->set_target_fps((int)fps);
				}
				return Variant();
			}
			if (r_error && *r_error == "") {
				*r_error = "Engine: unknown method '" + mname + "' (line " + itos(p_line) + ")";
			}
			return Variant();
		}
		if (seg == "GetFPS") {
			return Variant(Engine::get_singleton()->get_frames_per_second());
		}
		if (seg == "GetScreenSize") {
			return OS::get_singleton()->get_screen_size();
		}
		if (seg == "GetWindowSize") {
			return OS::get_singleton()->get_window_size();
		}
		if (r_error && *r_error == "") {
			*r_error = "Engine: unknown property '" + seg + "' (line " + itos(p_line) + ")";
		}
		return Variant();
	}
	return Variant();
}

/* Evaluates the material domain after "REF.Material.":
 *   Material.Color, Material.Metallic.Specular, Material.NormalMap.Enable,
 *   Material.Clear(), Material.Copy(other), ...
 */
static Variant eval_material_path(GCLScriptInstance *p_inst, Object *p_material, const Vector<String> &p_segments, int p_seg_idx, const Map<StringName, Variant> &p_locals, int p_line, String *r_error) {
	(void)p_inst;
	for (int s = p_seg_idx; s < p_segments.size(); s++) {
		String seg = p_segments[s].strip_edges();
		if (seg == "") {
			continue;
		}
		if (!p_material) {
			if (r_error) {
				*r_error = "Material: null material (line " + itos(p_line) + ")";
			}
			return Variant();
		}
		// Method call: Material.Clear() / Copy(x) / Save(p) / Load(p)
		int open = seg.find("(");
		if (open != -1 && seg.ends_with(")")) {
			String mname = seg.substr(0, open).strip_edges();
			String inside = seg.substr(open + 1, seg.length() - open - 2);
			Vector<String> parts = gcl_split_top_commas(inside);
			Array call_args;
			for (int k = 0; k < parts.size(); k++) {
				String part = parts[k].strip_edges();
				if (part == "") {
					continue;
				}
				call_args.push_back(gcl_eval_expr(part, p_locals, p_inst, p_line, r_error));
			}
			Variant rv;
			if (gcl_material_method(p_material, mname, call_args, rv, r_error, p_line)) {
				if (s == p_segments.size() - 1) {
					return rv;
				}
				p_material = rv.get_type() == Variant::OBJECT ? (Object *)rv : nullptr;
				continue;
			}
			if (r_error && *r_error == "") {
				*r_error = "Material: unknown method '" + mname + "' (line " + itos(p_line) + ")";
			}
			return Variant();
		}
		// Two-segment pair: Metallic.Specular, NormalMap.Enable, Detail.Albedo...
		if (s + 1 < p_segments.size()) {
			String next_seg = p_segments[s + 1].strip_edges();
			if (next_seg != "x" && next_seg != "y" && next_seg != "z") {
				Variant rv;
				if (gcl_material_resolve(p_material, seg, next_seg, rv)) {
					if (s + 2 >= p_segments.size()) {
						return rv;
					}
					p_material = rv.get_type() == Variant::OBJECT ? (Object *)rv : nullptr;
					s++;
					continue;
				}
			}
		}
		// Single segment
		Variant rv;
		if (gcl_material_resolve(p_material, seg, "", rv)) {
			if (s == p_segments.size() - 1) {
				return rv;
			}
			p_material = rv.get_type() == Variant::OBJECT ? (Object *)rv : nullptr;
			continue;
		}
		if (r_error && *r_error == "") {
			*r_error = "Material: unknown property '" + seg + "' (line " + itos(p_line) + ")";
		}
		return Variant();
	}
	return Variant();
}

static Variant eval_path_from(GCLScriptInstance *p_inst, Variant p_base, const Vector<String> &p_segments, int p_seg_idx, const Map<StringName, Variant> &p_locals, int p_line, String *r_error) {
	p_base = gcl_variant_resolve_node(p_inst, p_base);
	Object *obj = p_base;

	for (int s = p_seg_idx; s < p_segments.size(); s++) {
		String seg = p_segments[s].strip_edges();
		if (seg == "") {
			continue;
		}
		if (!obj) {
			if (r_error) {
				*r_error = "null object in member access '" + seg + "' (line " + itos(p_line) + ")";
			}
			return Variant();
		}
		// Vector3 component: .x .y .z
		if (p_base.get_type() == Variant::VECTOR3 && (seg == "x" || seg == "y" || seg == "z")) {
			Vector3 vec = p_base;
			return (seg == "x") ? Variant(vec.x) : (seg == "y") ? Variant(vec.y) : Variant(vec.z);
		}
		int open = seg.find("(");
		if (open != -1 && seg.ends_with(")")) {
			// method call
			String mname = seg.substr(0, open).strip_edges();
			String inside = seg.substr(open + 1, seg.length() - open - 2);
			Vector<String> parts = gcl_split_top_commas(inside);
			Array call_args;
			for (int k = 0; k < parts.size(); k++) {
				String part = parts[k].strip_edges();
				if (part == "") {
					continue;
				}
				call_args.push_back(gcl_eval_expr(part, p_locals, p_inst, p_line, r_error));
			}
			if (r_error && *r_error != "") {
				return Variant();
			}

			// GCL node methods (Find("name")) on Node objects
			{
				Node *node_obj = Object::cast_to<Node>(obj);
				if (node_obj) {
					Variant node_rv;
					if (gcl_node_method(node_obj, mname, call_args, node_rv, r_error, p_line)) {
						p_base = node_rv;
						obj = node_rv;
						if (s == p_segments.size() - 1) {
							return p_base;
						}
						if (!obj) {
							if (r_error) {
								*r_error = "cannot chain member '" + p_segments[s + 1] + "' on result of '" + mname + "' (line " + itos(p_line) + ")";
							}
							return Variant();
						}
						continue;
					}
					// Raycast methods on RayCast nodes (Skip, SkipList)
					if (gcl_raycast_method(node_obj, mname, call_args, node_rv, r_error, p_line)) {
						if (s == p_segments.size() - 1) {
							return node_rv;
						}
						p_base = node_rv;
						obj = node_rv;
						if (!obj) {
							if (r_error) {
								*r_error = "cannot chain member '" + p_segments[s + 1] + "' on result of '" + mname + "' (line " + itos(p_line) + ")";
							}
							return Variant();
						}
						continue;
					}
				}
			}

			// Input helpers
			if (obj == Input::get_singleton() && (mname == "IsDown" || mname == "IsPressed")) {
				if (call_args.size() > 0 && call_args[0].get_type() == Variant::STRING) {
					return Variant(gcl_is_down((String)call_args[0]));
				}
			}
			if (obj == Input::get_singleton() && mname == "IsKeyDown") {
				if (call_args.size() > 0 && call_args[0].get_type() == Variant::STRING) {
					String key_str = call_args[0];
					if (key_str.length() == 1) {
						int ch = key_str[0];
						int keycode = ch;
						if (ch >= 'a' && ch <= 'z') {
							keycode = ch - 32;
						} else if (ch >= 'A' && ch <= 'Z') {
							keycode = ch;
						} else if (ch >= '0' && ch <= '9') {
							keycode = ch;
						} else if (ch == ' ') {
							keycode = KEY_SPACE;
						}
						return Variant(Input::get_singleton()->is_key_pressed(keycode));
					}
					return Variant(Input::get_singleton()->is_action_pressed(key_str));
				}
			}

			// Not a real method: fail silently (no "Method not found" spam)
			if (!obj->has_method(StringName(mname))) {
				if (s == p_segments.size() - 1) {
					return Variant();
				}
				if (r_error) {
					*r_error = "cannot chain member '" + p_segments[s + 1] + "' on result of '" + mname + "' (line " + itos(p_line) + ")";
				}
				return Variant();
			}
			Variant rv = obj->callv(mname, call_args);
			p_base = rv;
			obj = rv;
			if (!obj && s < p_segments.size() - 1) {
				if (r_error) {
					*r_error = "cannot chain member '" + p_segments[s + 1] + "' on non-object result of '" + mname + "' (line " + itos(p_line) + ")";
				}
				return Variant();
			}
			continue;
		}

		// ---- property/segment read on a Node ----
		bool valid = false;
		Variant v;
		Node *node_obj = Object::cast_to<Node>(obj);
		if (node_obj) {
			// Material domain: REF.Material.<prop>
			if (seg == "Material") {
				Object *mat = nullptr;
				if (gcl_material_for_node(node_obj, mat, r_error, p_line)) {
					return eval_material_path(p_inst, mat, p_segments, s + 1, p_locals, p_line, r_error);
				}
			}
			// Raycast slots (IsColliding, GetBodyName, length) on RayCast nodes
			Variant ray_rv;
			if (gcl_raycast_segment(node_obj, seg, ray_rv, r_error, p_line)) {
				if (s == p_segments.size() - 1) {
					return ray_rv;
				}
				p_base = ray_rv;
				obj = ray_rv;
				if (!obj) {
					if (r_error) {
						*r_error = "cannot chain member '" + p_segments[s + 1] + "' on result of '" + seg + "' (line " + itos(p_line) + ")";
					}
					return Variant();
				}
				continue;
			}
			// Aliases: Name, Position, Rotation, Scale
			String alias = gcl_node_property_alias(seg);
			if (alias != "") {
				v = node_obj->get(alias, &valid);
				if (valid) {
					p_base = v;
					obj = v;
					continue;
				}
			}
			// GCL node segments: Childs, GetChild, Hide, Show, Free, Enable, Disable
			Variant seg_rv;
			if (gcl_node_segment(node_obj, seg, seg_rv, r_error, p_line)) {
				p_base = seg_rv;
				obj = seg_rv;
				if (s == p_segments.size() - 1) {
					return seg_rv;
				}
				if (!obj) {
					if (r_error) {
						*r_error = "cannot chain member '" + p_segments[s + 1] + "' on result of '" + seg + "' (line " + itos(p_line) + ")";
					}
					return Variant();
				}
				continue;
			}
		}
		v = obj->get(seg, &valid);
		if (!valid && obj->is_class("Node")) {
			if ((bool)obj->call("has_node", seg)) {
				v = obj->call("get_node", seg);
				if (v.get_type() != Variant::NIL) {
					valid = true;
				}
			}
			if (!valid) {
				Node *found = gcl_find_node_ci(Object::cast_to<Node>(obj), seg);
				if (found) {
					v = Variant(found);
					valid = true;
				}
			}
		}
		if (!valid) {
			// Parenthesis-less method call (self.GetBodyName;). has_method
			// first so unknown names fail silently.
			if (s == p_segments.size() - 1 && obj->has_method(StringName(seg))) {
				Variant rv = obj->callv(seg, Array());
				if (rv.get_type() != Variant::NIL) {
					return rv;
				}
			}
			return Variant(); // missing member/child: stop silently
		}
		v = gcl_variant_resolve_node(p_inst, v);
		p_base = v;
		obj = v;
	}
	return p_base;
}

/* -------------------------------------------------------------------- */
/*  Expression entry                                                     */
/* -------------------------------------------------------------------- */

Variant gcl_eval_expr(const String &p_expr, const Map<StringName, Variant> &p_locals, GCLScriptInstance *p_inst, int p_line, String *r_error) {
	String e = p_expr.strip_edges();
	if (e == "") {
		return Variant();
	}

	// ---- Operators (gcl_language.md) ----

	{
		int q = gcl_find_top_level(e, "?");
		if (q != -1) {
			int colon = -1;
			int depth = 0;
			bool in_str = false;
			bool in_char = false;
			for (int i = q + 1; i < e.length(); i++) {
				CharType c = e[i];
				if (c == '"') {
					in_str = !in_str;
				} else if (c == '\'') {
					in_char = !in_char;
				} else if (!in_str && !in_char) {
					if (c == '(') {
						depth++;
					} else if (c == ')') {
						depth--;
					} else if (c == ':' && depth == 0) {
						colon = i;
						break;
					}
				}
			}
			if (colon != -1) {
				String cond = e.substr(0, q).strip_edges();
				String a = e.substr(q + 1, colon - q - 1).strip_edges();
				String b = e.substr(colon + 1).strip_edges();
				return gcl_truthy_val(gcl_eval_expr(cond, p_locals, p_inst, p_line, r_error))
						? gcl_eval_expr(a, p_locals, p_inst, p_line, r_error)
						: gcl_eval_expr(b, p_locals, p_inst, p_line, r_error);
			}
		}
	}

	{
		int pos = gcl_find_top_level(e, "||");
		if (pos != -1) {
			Variant lv = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			if (gcl_truthy_val(lv)) {
				return Variant(true);
			}
			return Variant(gcl_truthy_val(gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error)));
		}
		pos = gcl_find_top_level(e, "&&");
		if (pos != -1) {
			Variant lv = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			if (!gcl_truthy_val(lv)) {
				return Variant(false);
			}
			return Variant(gcl_truthy_val(gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error)));
		}
	}

	{
		int pos = gcl_find_top_level(e, "==");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error);
			return Variant(gcl_compare_vals(a, b) == 0);
		}
		pos = gcl_find_top_level(e, "!=");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error);
			return Variant(gcl_compare_vals(a, b) != 0);
		}
	}

	{
		int pos = gcl_find_top_level(e, "<=");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error);
			return Variant(gcl_compare_vals(a, b) <= 0);
		}
		pos = gcl_find_top_level(e, ">=");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error);
			return Variant(gcl_compare_vals(a, b) >= 0);
		}
		pos = gcl_find_top_level(e, "<");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return Variant(gcl_compare_vals(a, b) < 0);
		}
		pos = gcl_find_top_level(e, ">");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return Variant(gcl_compare_vals(a, b) > 0);
		}
	}

	{
	int pos;
	pos = gcl_find_top_level(e, String("&"));
	if (pos != -1) {
		Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
		Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
		return Variant((int)((int)a & (int)b));
	}
pos = gcl_find_top_level(e, String("|"));
	if (pos != -1) {
		Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
		Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
		return Variant((int)((int)a | (int)b));
	}
pos = gcl_find_top_level(e, String("^"));
	if (pos != -1) {
		Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
		Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
		return Variant((int)((int)a ^ (int)b));
	}
pos = gcl_find_top_level(e, String("<<"));
	if (pos != -1) {
		Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
		Variant b = gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error);
		return Variant((int)((int)a << (int)b));
	}
pos = gcl_find_top_level(e, String(">>"));
	if (pos != -1) {
		Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
		Variant b = gcl_eval_expr(e.substr(pos + 2).strip_edges(), p_locals, p_inst, p_line, r_error);
		return Variant((int)((int)a >> (int)b));
	}
	}
	if (e.length() >= 1 && e[0] == '~') {
		return Variant(~(int)gcl_eval_expr(e.substr(1).strip_edges(), p_locals, p_inst, p_line, r_error));
	}
if (e.length() >= 1 && e[0] == '!' && (e.length() == 1 || e[1] != '=')) {
		return Variant(!gcl_truthy_val(gcl_eval_expr(e.substr(1).strip_edges(), p_locals, p_inst, p_line, r_error)));
	}

	{
		int pos = gcl_find_top_level(e, "+");
		if (pos != -1 && e.substr(0, pos).strip_edges() != "") {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return gcl_arith(a, b, "+");
		}
		pos = gcl_find_top_level(e, "-");
		if (pos != -1 && e.substr(0, pos).strip_edges() != "") {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return gcl_arith(a, b, "-");
		}
	}

	{
		int pos = gcl_find_top_level(e, "*");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return gcl_arith(a, b, "*");
		}
		pos = gcl_find_top_level(e, "/");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return gcl_arith(a, b, "/");
		}
		pos = gcl_find_top_level(e, "%");
		if (pos != -1) {
			Variant a = gcl_eval_expr(e.substr(0, pos).strip_edges(), p_locals, p_inst, p_line, r_error);
			Variant b = gcl_eval_expr(e.substr(pos + 1).strip_edges(), p_locals, p_inst, p_line, r_error);
			return gcl_arith(a, b, "%");
		}
	}

	if (e.length() >= 2 && e[0] == '(' && e[e.length() - 1] == ')') {
		int depth = 0;
		bool outer = true;
		for (int i = 0; i < e.length(); i++) {
			if (e[i] == '(') {
				depth++;
			} else if (e[i] == ')') {
				depth--;
				if (depth == 0 && i < e.length() - 1) {
					outer = false;
					break;
				}
			}
		}
		if (outer && depth == 0) {
			return gcl_eval_expr(e.substr(1, e.length() - 2), p_locals, p_inst, p_line, r_error);
		}
	}

	// Global helper: GetNode("name") - whole-tree lookup by name
	if (e.begins_with("GetNode(") && e.ends_with(")")) {
		String inside = e.substr(8, e.length() - 9).strip_edges();
		if (inside.length() >= 2 && inside[0] == '"' && inside[inside.length() - 1] == '"') {
			return gcl_node_by_name(p_inst, inside.substr(1, inside.length() - 2), r_error, p_line);
		}
	}

	// GCL function call: Name(args) — resolves script functions like
	// gcl_language.md GCL 101 ("print_this_global();") inside expressions.
	{
		int open = e.find("(");
		if (open != -1 && e.ends_with(")") && e[0] != '.' && e[0] != '$' && e[0] != '@') {
			String fname = e.substr(0, open).strip_edges();
			if (fname != "" && fname.find(" ") == -1 && fname.find(".") == -1 && fname.find("[") == -1 && fname.find("\"") == -1 && fname.find("'") == -1) {
				if (p_inst && p_inst->has_function(StringName(fname))) {
					String inside = e.substr(open + 1, e.length() - open - 2);
					Vector<String> parts = gcl_split_top_commas(inside);
					Vector<Variant> args;
					for (int k = 0; k < parts.size(); k++) {
						String part = parts[k].strip_edges();
						if (part == "") {
							continue;
						}
						args.push_back(gcl_eval_expr(part, p_locals, p_inst, p_line, r_error));
					}
					return p_inst->call_function(StringName(fname), args);
				}
			}
		}
	}

	// Member path: self.X.Y, X.Y, $"path".X, FindNode("n").X
	Vector<String> segments = gcl_split_member_path(e);
	if (segments.size() > 1) {
		String base_seg = segments[0].strip_edges();
		Variant base;
		if (base_seg == "Engine") {
			// Engine is not an Object in this fork; handle it directly.
			return eval_engine_path(segments, 1, p_locals, p_inst, p_line, r_error);
		} else if (base_seg == "self") {
			base = p_inst->get_owner();
		} else if (base_seg == "Input") {
			base = Input::get_singleton();
		} else if (base_seg.begins_with("$\"") && base_seg.ends_with("\"")) {
			String path = base_seg.substr(2, base_seg.length() - 3);
			base = gcl_resolve_path(p_inst, path, r_error, p_line);
		} else if (base_seg.begins_with("GetNode(") && base_seg.ends_with(")")) {
			String inside = base_seg.substr(8, base_seg.length() - 9).strip_edges();
			String name;
			if (inside.length() >= 2 && inside[0] == '"' && inside[inside.length() - 1] == '"') {
				name = inside.substr(1, inside.length() - 2);
			} else {
				name = Variant(gcl_eval_expr(inside, p_locals, p_inst, p_line, r_error)).operator String();
			}
			base = gcl_node_by_name(p_inst, name, r_error, p_line);
		} else if (base_seg.begins_with("FindNode(") && base_seg.ends_with(")")) {
			String inside = base_seg.substr(9, base_seg.length() - 10).strip_edges();
			String name;
			if (inside.length() >= 2 && inside[0] == '"' && inside[inside.length() - 1] == '"') {
				name = inside.substr(1, inside.length() - 2);
			} else {
				name = Variant(gcl_eval_expr(inside, p_locals, p_inst, p_line, r_error)).operator String();
			}
			base = gcl_find_node(p_inst, name, r_error, p_line);
		} else {
			const Map<StringName, Variant>::Element *L = p_locals.find(StringName(base_seg));
			if (L) {
				base = L->get();
			} else {
				Variant v;
				if (p_inst->get(StringName(base_seg), v)) {
					base = v;
				}
			}
			if (base.get_type() == Variant::NIL && p_inst->get_owner()) {
				Object *owner = p_inst->get_owner();
				if ((bool)owner->call("has_node", base_seg)) {
					base = owner->call("get_node", base_seg);
				}
			}
		}
		return eval_path_from(p_inst, base, segments, 1, p_locals, p_line, r_error);
	}

	// String / char literals
	if (e.length() >= 2 && e[0] == '"' && e[e.length() - 1] == '"') {
		return Variant(e.substr(1, e.length() - 2));
	}
	if (e.length() >= 2 && e[0] == '\'' && e[e.length() - 1] == '\'') {
		return Variant(e.substr(1, e.length() - 2));
	}
	// Numbers
	if (e.is_numeric() || e.is_valid_float()) {
		if (e.find(".") != -1 || e.find("e") != -1 || e.find("E") != -1) {
			return Variant(e.to_float());
		}
		return Variant(e.to_int());
	}
	// Booleans / null
	if (e == "true") {
		return Variant(true);
	}
	if (e == "false") {
		return Variant(false);
	}
	if (e == "NULL" || e == "null" || e == "nullptr") {
		return Variant();
	}
	// Array literal {"a","b"}
	if (e.begins_with("{") && e.ends_with("}")) {
		String inside = e.substr(1, e.length() - 2);
		Vector<String> parts = gcl_split_top_commas(inside);
		Array arr;
		for (int i = 0; i < parts.size(); i++) {
			String part = parts[i].strip_edges();
			if (part == "") {
				continue;
			}
			arr.push_back(gcl_eval_expr(part, p_locals, p_inst, p_line, r_error));
		}
		return arr;
	}
	// Variable read
	const Map<StringName, Variant>::Element *L = p_locals.find(StringName(e));
	if (L) {
		return L->get();
	}
	Variant v;
	if (p_inst->get(StringName(e), v)) {
		return v;
	}
	return Variant();
}

/* -------------------------------------------------------------------- */
/*  Member path write                                                    */
/* -------------------------------------------------------------------- */

void gcl_write_member(GCLScriptInstance *p_inst, const String &p_target, const Variant &p_value, const Map<StringName, Variant> &p_locals, int p_line, String *r_error) {
	Vector<String> segments = gcl_split_member_path(p_target);
	if (segments.size() < 2) {
		return;
	}
	String base_seg = segments[0].strip_edges();
	Variant base;
	if (base_seg == "self") {
		base = p_inst->get_owner();
	} else if (base_seg == "Input") {
		base = Input::get_singleton();
	} else if (base_seg.begins_with("$\"") && base_seg.ends_with("\"")) {
		String path = base_seg.substr(2, base_seg.length() - 3);
		base = gcl_resolve_path(p_inst, path, r_error, p_line);
	} else if (base_seg.begins_with("GetNode(") && base_seg.ends_with(")")) {
		String inside = base_seg.substr(8, base_seg.length() - 9).strip_edges();
		String name;
		if (inside.length() >= 2 && inside[0] == '"' && inside[inside.length() - 1] == '"') {
			name = inside.substr(1, inside.length() - 2);
		} else {
			name = Variant(gcl_eval_expr(inside, p_locals, p_inst, p_line, r_error)).operator String();
		}
		base = gcl_node_by_name(p_inst, name, r_error, p_line);
	} else if (base_seg.begins_with("FindNode(") && base_seg.ends_with(")")) {
		String inside = base_seg.substr(9, base_seg.length() - 10).strip_edges();
		String name;
		if (inside.length() >= 2 && inside[0] == '"' && inside[inside.length() - 1] == '"') {
			name = inside.substr(1, inside.length() - 2);
		} else {
			name = Variant(gcl_eval_expr(inside, p_locals, p_inst, p_line, r_error)).operator String();
		}
		base = gcl_find_node(p_inst, name, r_error, p_line);
	} else {
		const Map<StringName, Variant>::Element *L = p_locals.find(StringName(base_seg));
		if (L) {
			base = L->get();
		} else {
			Variant v;
			if (p_inst->get(StringName(base_seg), v)) {
				base = v;
			}
		}
		if (base.get_type() == Variant::NIL && p_inst->get_owner()) {
			Object *owner = p_inst->get_owner();
			if ((bool)owner->call("has_node", base_seg)) {
				base = owner->call("get_node", base_seg);
			}
		}
	}
	// Engine.SetFPS = <n>  (parenthesis-less setter, gcl.md "temel komutlar")
	if (base_seg == "Engine" && segments.size() == 2) {
		String set_seg = segments[1].strip_edges();
		if (set_seg == "SetFPS") {
			Engine::get_singleton()->set_target_fps((int)p_value);
			return;
		}
	}

	Object *obj = base;
	if (!obj) {
		if (r_error && *r_error == "") {
			*r_error = "cannot write to null object '" + base_seg + "' (line " + itos(p_line) + ")";
		}
		return;
	}

	// Material domain write: REF.Material.<prop> = v
	{
		Node *base_node = Object::cast_to<Node>(obj);
		if (base_node && segments.size() >= 3 && segments[1].strip_edges() == "Material") {
			Object *mat = nullptr;
			if (gcl_material_for_node(base_node, mat, r_error, p_line)) {
				String p1 = segments[2].strip_edges();
				String p2 = segments.size() >= 4 ? segments[3].strip_edges() : "";
				if (gcl_material_resolve_write(mat, p1, p2, p_value)) {
					return;
				}
			}
		}
	}

	for (int s = 1; s < segments.size() - 1; s++) {
		String seg = segments[s].strip_edges();

		// Component write on a GCL alias: REF.Rotation.y = v, REF.Position.x = v
		if (s + 1 == segments.size() - 1) {
			String next_seg = segments[s + 1].strip_edges();
			if (next_seg == "x" || next_seg == "y" || next_seg == "z") {
				String alias = gcl_node_property_alias(seg);
				if (alias != "") {
					Vector3 vec = obj->get(alias);
					int comp = (next_seg == "x") ? 0 : (next_seg == "y") ? 1 : 2;
					vec[comp] = (float)p_value;
					obj->set(alias, vec);
					return;
				}
			}
		}

		bool valid = false;
		Variant v = obj->get(seg, &valid);
		// GCL alias on an intermediate segment (Name/Position/Rotation/Scale)
		if (!valid && obj->is_class("Node")) {
			String alias = gcl_node_property_alias(seg);
			if (alias != "") {
				v = obj->get(alias, &valid);
			}
		}
		if (!valid && obj->is_class("Node")) {
			if ((bool)obj->call("has_node", seg)) {
				v = obj->call("get_node", seg);
				if (v.get_type() != Variant::NIL) {
					valid = true;
				}
			}
			if (!valid) {
				Node *found = gcl_find_node_ci(Object::cast_to<Node>(obj), seg);
				if (found) {
					v = Variant(found);
					valid = true;
				}
			}
		}
		if (!valid) {
			if (r_error && *r_error == "") {
				*r_error = "cannot write through '" + seg + "' (line " + itos(p_line) + ")";
			}
			return;
		}
		obj = v;
	}
	String final_seg = segments[segments.size() - 1].strip_edges();

	// Raycast write: REF.length = <float> (cast_to length)
	{
		Node *final_node = Object::cast_to<Node>(obj);
		if (final_node && gcl_raycast_write(final_node, final_seg, p_value)) {
			return;
		}
	}

	String final_alias = gcl_node_property_alias(final_seg);
	if (final_alias != "") {
		final_seg = final_alias;
	}
	obj->set(final_seg, p_value);
}
