/**************************************************************************/
/*  executor_class.h                                                     */
/**************************************************************************/
/*  GCL Executor - class destegi (gcl_language.md -> class).              */
/*  - "class FATHER() { ... }" bildirimini kaydeder                       */
/*  - "FATHER ThisFather = FATHER" ornek (instance) olusturur             */
/*  - "ThisFather.Call" (@return metodu) / "ThisChild.talk()" cagrilari   */
/*  Amac: her executor dosyasini <800 satir tutmak.                       */
/**************************************************************************/

#ifndef GCL_EXECUTOR_CLASS_H
#define GCL_EXECUTOR_CLASS_H

#include "executor_flow.h"

namespace gcl {

namespace executor_class {

/* "__gcl_method:<class>:<method>" sentineli. executor_core::evaluate_expr
   "@return"li metot erisiminde bunu uretir; burasi gercek degerle degistirir. */
inline bool resolve_class_method_value(const String &p_sentinel, const Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, Variant &r_out) {
	if (!p_sentinel.begins_with("__gcl_method:")) {
		return false;
	}
	String rest = p_sentinel.substr(14);
	int sep = rest.find(":");
	if (sep == -1) {
		return false;
	}
	String class_name = rest.substr(0, sep);
	String method_name = rest.substr(sep + 1);
	StringName cn = StringName(class_name);
	if (!p_types.classes.has(cn)) {
		return false;
	}
	Dictionary cls = p_types.classes[cn];
	if (!cls.has("methods")) {
		return false;
	}
		Dictionary methods = cls["methods"];
		if (!methods.has(method_name)) {
			return false;
		}
		Dictionary m = methods[method_name];
		bool is_return = m.has("is_return") && bool(m["is_return"]);
		if (!is_return) {
			return false;
		}

	String body = m["body"];
	Map<StringName, Variant> scope;
	scope[StringName("self")] = Variant();
	int ex = 0;
	Variant rv;
	executor_run_ex(body, scope, p_types, ex, &rv);
	if (ex == 3) {
		r_out = rv;
		return true;
	}
	return false;
}

/* "class NAME (BASE) { ... }" ifadesini kaydeder. p_types.classes'a
   Dictionary { "base": String, "methods": { ad -> {sig, body, is_return} } } yazar. */
inline void handle_class_stmt(const String &p_stmt, GCLTypeRegistry &p_types) {
	String s = p_stmt.strip_edges();
	if (!s.begins_with("class ")) {
		return;
	}
	s = s.substr(6).strip_edges();

	int open_paren = s.find("(");
	String class_name;
	String base_name;
	if (open_paren == -1) {
		class_name = s;
	} else {
		class_name = s.substr(0, open_paren).strip_edges();
		int close_paren = s.find(")", open_paren);
		if (close_paren != -1) {
			base_name = s.substr(open_paren + 1, close_paren - open_paren - 1).strip_edges();
		}
	}
	if (!executor_core::is_valid_identifier(class_name)) {
		return;
	}

	int open_brace = s.find("{");
	if (open_brace == -1) {
		return;
	}
	int depth = 0;
	int close_brace = -1;
	for (int i = open_brace; i < s.length(); i++) {
		if (s[i] == '{') {
			depth++;
		} else if (s[i] == '}') {
			depth--;
			if (depth == 0) {
				close_brace = i;
				break;
			}
		}
	}
	if (close_brace == -1) {
		return;
	}
	String body = s.substr(open_brace + 1, close_brace - open_brace - 1);

	/* Method'lari topla: imza + govde + @return isareti.
	   Govde tek satira duzlesmis (K&R { } satir ici) olabilir; karakter bazli
	   dengeli tarama kullanilir, birden cok metot ayni satirda da calisir. */
	Dictionary methods;
	int pos = 0;
	const int BL = body.length();
	String pending_return = "";
	while (pos < BL) {
		int le = body.find("\n", pos);
		if (le == -1) {
			le = BL;
		}
		String line = body.substr(pos, le - pos).strip_edges();
		if (line == "@return") {
			pending_return = "@return";
			pos = le + 1;
			continue;
		}
		if (!line.empty()) {
			int ob = line.find("{");
			String sig_part = (ob == -1) ? line : line.substr(0, ob).strip_edges();
			if (sig_part.find("(") != -1) {
				/* @return ayni satira duzlesmis olabilir: "@return int Age() {" */
				bool inline_ret = sig_part.find("@return") != -1;
				if (inline_ret) {
					sig_part = sig_part.replace("@return", "").strip_edges();
				}
				String body_part;
				int body_end = -1;
				if (ob != -1) {
					/* K&R: { ... } imza satirinda veya sonraki satirlarda.
					   DİKKAT: ob, strip edilmis line uzerindeki indekstir; body
					   uzerindeki gercek '{' konumuyla kayabilir. Govde acilisini
					   body uzerinde DOGRUDAN ara (kayma hatasi class'in hic
					   kaydedilmemesine yol acar). */
					int bopen = -1;
					int scan = pos;
					while (scan < BL) {
						if (body[scan] == '{') {
							bopen = scan;
							break;
						}
						scan++;
					}
					if (bopen == -1) {
						pos = le + 1;
						continue;
					}
					int bd = 1;
					int ci = bopen + 1;
					while (ci < BL) {
						if (body[ci] == '{') {
							bd++;
						} else if (body[ci] == '}') {
							bd--;
							if (bd == 0) {
								body_end = ci;
								break;
							}
						}
						ci++;
					}
					if (body_end != -1) {
						body_part = body.substr(bopen + 1, body_end - (bopen + 1)).strip_edges();
						pos = body_end + 1;
					} else {
						pos = le + 1;
						continue;
					}
				} else {
					/* Allman: govde sonraki { ... } blogu */
					int bopen = -1;
					int search = le + 1;
					while (search < BL && bopen == -1) {
						int q = body.find("{", search);
						if (q != -1) {
							bopen = q;
							break;
						}
						int sl = body.find("\n", search);
						if (sl == -1) {
							sl = BL;
						}
						search = sl + 1;
					}
					if (bopen == -1) {
						pos = le + 1;
						continue;
					}
					int bd = 1;
					int ci = bopen + 1;
					while (ci < BL) {
						if (body[ci] == '{') {
							bd++;
						} else if (body[ci] == '}') {
							bd--;
							if (bd == 0) {
								body_end = ci;
								break;
							}
						}
						ci++;
					}
					if (body_end == -1) {
						pos = le + 1;
						continue;
					}
					body_part = body.substr(bopen + 1, body_end - (bopen + 1)).strip_edges();
					pos = body_end + 1;
				}

				bool is_head = sig_part.find("head(") != -1 || sig_part.begins_with("void head ") || sig_part == "void head";
				Dictionary m;
				m["sig"] = sig_part;
				m["body"] = body_part;
				m["is_return"] = (pending_return == "@return") || inline_ret;
				m["is_head"] = is_head;
				String fn_name = sig_part;
				int fn_op = fn_name.find("(");
				if (fn_op != -1) {
					fn_name = fn_name.substr(0, fn_op).strip_edges();
				}
				int fn_last_space = fn_name.rfind(" ");
				if (fn_last_space != -1) {
					fn_name = fn_name.substr(fn_last_space + 1);
				}
				pending_return = "";
				if (!fn_name.empty()) {
					methods[fn_name] = m;
				}
				continue; /* pos zaten govde sonrasina ilerletildi */
			}
		}
		if (le == BL) {
			break;
		}
		pos = le + 1;
	}

	Dictionary cls;
	cls["base"] = base_name;
	cls["methods"] = methods;
	p_types.classes[StringName(class_name)] = cls;
}

/* "FATHER" tipinden ornek uretir; head metodu varsa calistirir.
   p_args: "CHILD(ThisFather)" icin kurucu argumanlari. */
inline Variant make_class_instance(const String &p_class_name, const Vector<String> &p_args, const Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types) {
	StringName cn = StringName(p_class_name);
	if (!p_types.classes.has(cn)) {
		return Variant();
	}
	Dictionary cls = p_types.classes[cn];
	Dictionary methods = cls.has("methods") ? (Dictionary)cls["methods"] : Dictionary();

	Dictionary inst;
	inst["__class"] = p_class_name;
	String base_name = cls.has("base") ? (String)cls["base"] : String();
	inst["__base"] = base_name;

	/* Kalitim (basit): base class'in metodlarini da ekle. */
	StringName base_sn = StringName(base_name);
	if (!base_name.empty() && p_types.classes.has(base_sn)) {
		Dictionary base_cls = p_types.classes[base_sn];
		if (base_cls.has("methods")) {
			Dictionary base_methods = base_cls["methods"];
			Array mk = base_methods.keys();
			for (int bi = 0; bi < mk.size(); bi++) {
				String bname = String(mk[bi]);
				if (!methods.has(bname)) {
					methods[bname] = base_methods[bname];
				}
			}
		}
	}
	inst["__methods"] = methods;

	/* head varsa calistir: alanlari ornege yazar. */
	if (methods.has("head")) {
		Dictionary head = methods["head"];
		String body = head["body"];
		Map<StringName, Variant> scope;
		scope[StringName("self")] = inst;
		Vector<String> param_names;
		{
			String sig = head["sig"];
			int op_ = sig.find("(");
			int cp_ = sig.rfind(")");
			if (op_ != -1 && cp_ != -1) {
				param_names = executor_flow::parse_func_params(sig.substr(op_ + 1, cp_ - op_ - 1));
			}
		}
		for (int i = 0; i < param_names.size(); i++) {
			Variant v;
			if (i < p_args.size()) {
				v = executor_core::initialize_value(p_args[i], p_members);
			}
			scope[StringName(param_names[i])] = v;
		}
		int ex = 0;
		Variant rv;
		executor_run_ex(body, scope, p_types, ex, &rv);

		/* scope'taki alanlari ornege aktar (__* haric) */
		for (const Map<StringName, Variant>::Element *E = scope.front(); E; E = E->next()) {
			StringName k = E->key();
			String ks = String(k);
			if (!ks.begins_with("__")) {
				inst[k] = E->value();
			}
		}
	}

	return inst;
}

/* "obj.Method(...)" veya "obj.Field" cagrisini isler (ornek Dictionary). */
inline bool resolve_class_call(const String &p_line, const Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, Variant *r_retval) {
	String s = p_line.strip_edges().replace(";", "").strip_edges();
	if (s.empty() || s.begins_with("PrintF") || s.begins_with("print")) {
		return false;
	}

	int dot = s.find(".");
	if (dot <= 0 || dot + 1 >= s.length()) {
		return false;
	}
	String obj_name = s.substr(0, dot).strip_edges();
	String rest = s.substr(dot + 1);
	if (!p_members.has(StringName(obj_name))) {
		return false;
	}
	Variant obj = p_members.find(StringName(obj_name))->get();
	if (obj.get_type() != Variant::DICTIONARY) {
		return false;
	}
	Dictionary inst = obj;
	if (!inst.has("__class")) {
		return false;
	}

	/* metot cagrisi: Method(args) */
	int open_p = rest.find("(");
	if (open_p != -1) {
		int close_p = rest.rfind(")");
		if (close_p == -1 || close_p < open_p) {
			return false;
		}
		String mname = rest.substr(0, open_p).strip_edges();
		String args_region = rest.substr(open_p + 1, close_p - open_p - 1);
		Vector<String> arg_strings;
		int a_start = 0;
		for (int j = 0; j <= args_region.length(); j++) {
			if (j == args_region.length() || args_region[j] == ',') {
				String arg = args_region.substr(a_start, j - a_start).strip_edges();
				if (!arg.empty()) {
					arg_strings.push_back(arg);
				}
				a_start = j + 1;
			}
		}

		Dictionary methods = inst["__methods"];
		if (!methods.has(mname)) {
			return false;
		}
		Dictionary m = methods[mname];
		String body = m["body"];
		String sig = m["sig"];

		Map<StringName, Variant> scope;
		scope[StringName("self")] = inst;
		Vector<String> param_names;
		{
			int op_ = sig.find("(");
			int cp_ = sig.rfind(")");
			if (op_ != -1 && cp_ != -1) {
				param_names = executor_flow::parse_func_params(sig.substr(op_ + 1, cp_ - op_ - 1));
			}
		}
		for (int i = 0; i < param_names.size(); i++) {
			Variant v;
			if (i < arg_strings.size()) {
				v = executor_core::initialize_value(arg_strings[i], p_members);
			}
			scope[StringName(param_names[i])] = v;
		}

		/* instance alanlarini scope'a kopyala */
		Array inst_keys = inst.keys();
		for (int ki = 0; ki < inst_keys.size(); ki++) {
			String ks = String(inst_keys[ki]);
			if (!ks.begins_with("__")) {
				scope[StringName(ks)] = inst[ks];
			}
		}

		p_types.call_depth++;
		int ex = 0;
		Variant rv;
		executor_run_ex(body, scope, p_types, ex, &rv);
		p_types.call_depth--;
		if (r_retval && ex == 3) {
			*r_retval = rv;
		}
		return true;
	}

	/* property erisimi: obj.Field (head alanı veya @return metodu) */
	String fname = rest.strip_edges();
	if (inst.has(fname)) {
		if (r_retval) {
			*r_retval = inst[fname];
		}
		return true;
	}
	Dictionary methods = inst["__methods"];
	if (methods.has(fname)) {
		Dictionary m = methods[fname];
		bool is_return = m.has("is_return") && bool(m["is_return"]);
		if (is_return) {
			String body = m["body"];
			Map<StringName, Variant> scope;
			scope[StringName("self")] = inst;
			Array inst_keys = inst.keys();
			for (int ki = 0; ki < inst_keys.size(); ki++) {
				String ks = String(inst_keys[ki]);
				if (!ks.begins_with("__")) {
					scope[StringName(ks)] = inst[ks];
				}
			}
			p_types.call_depth++;
			int ex = 0;
			Variant rv;
			executor_run_ex(body, scope, p_types, ex, &rv);
			p_types.call_depth--;
			if (ex == 3 && r_retval) {
				*r_retval = rv;
			}
			return true;
		}
	}
	return false;
}

/* PrintF/print cagrisini isler; argumanlarin class alan/metot erisimini de
   cozer ("ThisFather.Call", "ThisChild.talk()"). Cagri bulunamazsa false. */
inline bool handle_call_full(const String &p_line, const Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types) {
	for (int pass = 0; pass < 2; pass++) {
		const String fn = pass == 0 ? "PrintF" : "print";
		int pi = -1;
		if (!executor_core::find_call_word(p_line, fn, pi)) {
			continue;
		}

		int open = p_line.find("(", pi + fn.length());
		if (open == -1) {
			continue;
		}
		int depth = 0;
		bool in_str = false;
		int close = -1;
		for (int j = open; j < p_line.length(); j++) {
			CharType c = p_line[j];
			if (c == '"') {
				in_str = !in_str;
			} else if (!in_str && c == '(') {
				depth++;
			} else if (!in_str && c == ')') {
				depth--;
				if (depth == 0) {
					close = j;
					break;
				}
			}
		}
		if (close == -1) {
			continue;
		}
		int q1 = p_line.find("\"", open + 1);
		if (q1 == -1 || q1 > close) {
			continue;
		}
		int q2 = p_line.find("\"", q1 + 1);
		if (q2 == -1 || q2 > close) {
			continue;
		}
		String fmt = p_line.substr(q1 + 1, q2 - q1 - 1);

		String args_region = p_line.substr(q2 + 1, close - q2 - 1);
		Vector<String> arg_strings;
		int a_start = 0;
		for (int j = 0; j <= args_region.length(); j++) {
			if (j == args_region.length() || args_region[j] == ',') {
				String arg = args_region.substr(a_start, j - a_start).strip_edges();
				if (!arg.empty()) {
					arg_strings.push_back(arg);
				}
				a_start = j + 1;
			}
		}

		Vector<Variant> args;
		for (int k = 0; k < arg_strings.size(); k++) {
			String arg = arg_strings[k].strip_edges();
			Variant v = executor_core::initialize_value(arg, p_members);
			if (v.get_type() == Variant::NIL) {
				/* sizeof(int), aritmetik, ternary: initialize_value cözemezse
				   solve_arith dener (ops_initialize_value sizeof'u bilir). */
				Variant av = executor_ops::solve_arith(arg, p_members);
				if (av.get_type() != Variant::NIL) {
					v = av;
				} else {
					Variant cv;
					if (resolve_class_call(arg, p_members, p_types, &cv)) {
						v = cv;
					}
				}
			}
			args.push_back(v);
		}

		int consumed = 0;
		String out = executor_core::apply_printf(fmt, args, consumed);
		if (pass == 0) {
			print_string(out);
		} else {
			print_line(out);
		}
		return true;
	}

	return false;
}

} // namespace executor_class

} // namespace gcl

#endif // GCL_EXECUTOR_CLASS_H
