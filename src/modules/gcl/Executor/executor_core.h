/**************************************************************************/
/*  executor_core.h                                                      */
/**************************************************************************/
/*  GCL Executor - Deger/literal/tip islemleri.                           */
/*  printf formatlama executor_format.h'da, kontrol akisi                 */
/*  executor_flow.h'da, satir dongusu executor.cpp'de.                    */
/*  Amac: her executor dosyasini <800 satir tutmak.                       */
/**************************************************************************/

#ifndef GCL_EXECUTOR_CORE_H
#define GCL_EXECUTOR_CORE_H

#include "executor_format.h"

#include "core/print_string.h"

#include "executor_ops.h"

namespace gcl {

namespace executor_core {

using executor_format::is_ident_start;
using executor_format::is_ident_char;
using executor_format::is_number_keyword;
using executor_format::split_declaration;
using executor_format::is_valid_identifier;
using executor_format::variant_int;
using executor_format::variant_real;
using executor_format::variant_str;
using executor_format::apply_printf;
using executor_ops::solve_arith;

/* Ust seviye ayirici: {} () [] ve "" icindeki ayirac karakterlerini saymaz. */
inline Vector<String> split_top_level(const String &p_text, CharType p_sep) {
	Vector<String> out;
	int depth = 0;
	int start = 0;
	const int L = p_text.length();
	bool in_str = false;
	for (int i = 0; i < L; i++) {
		CharType c = p_text[i];
		if (c == '"') {
			in_str = !in_str;
		} else if (!in_str) {
			if (c == '{' || c == '(' || c == '[') {
				depth++;
			} else if (c == '}' || c == ')' || c == ']') {
				depth--;
			} else if (c == p_sep && depth == 0) {
				out.push_back(p_text.substr(start, i - start).strip_edges());
				start = i + 1;
			}
		}
	}
	String last = p_text.substr(start).strip_edges();
	if (!last.empty() || !out.empty()) {
		out.push_back(last);
	}
	return out;
}

/* "A.B[0].C" zincir ifadesini degerlendirir. */
inline Variant evaluate_expr(const String &p_expr, const Map<StringName, Variant> &p_members) {
	String expr = p_expr.strip_edges();
	if (expr.empty() || !is_ident_start(expr[0])) {
		return Variant();
	}

	int i = 0;
	const int L = expr.length();
	while (i < L && is_ident_char(expr[i])) {
		i++;
	}
	String first = expr.substr(0, i);
	if (!p_members.has(StringName(first))) {
		return Variant();
	}
	Variant cur = p_members.find(StringName(first))->get();

	while (i < L) {
		/* "." veya "->" ile alan erisimi */
		if ((expr[i] == '.' || (expr[i] == '-' && i + 1 < L && expr[i + 1] == '>')) &&
				i + 1 < L && is_ident_start(expr[i + 1])) {
			int arrow_skip = (expr[i] == '-') ? 2 : 1;
			int j = i + arrow_skip;
			String field;
			if (j < L && is_ident_start(expr[j])) {
				int k = j;
				while (k < L && is_ident_char(expr[k])) {
					k++;
				}
				field = expr.substr(j, k - j);
				j = k;
			} else {
				return Variant();
			}
			if (cur.get_type() == Variant::DICTIONARY) {
				Dictionary d = cur;
				if (d.has(field)) {
					cur = d[field];
				} else {
					return Variant();
				}
			} else {
				return Variant();
			}
			i = j;
		} else if (expr[i] == '[') {
			int close = expr.find("]", i + 1);
			if (close == -1) {
				return Variant();
			}
			String idx = expr.substr(i + 1, close - i - 1).strip_edges();
			if (cur.get_type() == Variant::ARRAY) {
				Array arr = cur;
				int n = idx.to_int();
				if (n >= 0 && n < arr.size()) {
					cur = arr[n];
				} else {
					return Variant();
				}
			} else {
				return Variant();
			}
			i = close + 1;
		} else if (expr[i] == ' ' || expr[i] == '\t') {
			i++;
		} else {
			break;
		}
	}

	return cur;
}

/* RHS degerini cozer: true/false/null, "str", 'c', {..}, sayi, degisken, zincir. */
inline Variant initialize_value(const String &p_rhs, const Map<StringName, Variant> &p_members) {
	String rhs = p_rhs.strip_edges();
	if (rhs == "true") {
		return true;
	} else if (rhs == "false") {
		return false;
	} else if (rhs == "null" || rhs.empty()) {
		return Variant();
	} else if (rhs.length() >= 2 && rhs[0] == '"' && rhs[rhs.length() - 1] == '"') {
		return rhs.substr(1, rhs.length() - 2);
	} else if (rhs.length() >= 3 && rhs[0] == '\'' && rhs[rhs.length() - 1] == '\'') {
		String chr = rhs.substr(1, rhs.length() - 2);
		return chr.empty() ? Variant() : Variant(chr.substr(0, 1));
	} else if (!rhs.empty() && rhs[0] == '{') {
		if (rhs[rhs.length() - 1] == '}') {
			return rhs.substr(1, rhs.length() - 2).strip_edges();
		}
		return rhs.substr(1).strip_edges();
	} else if (rhs.find(".") != -1 || rhs.find("[") != -1) {
		/* "30.3000" gibi noktali sayi mi? (identifier degilse evaluate_expr 0 doner) */
		if (!is_ident_start(rhs[0])) {
			return rhs.to_double();
		}
		return evaluate_expr(rhs, p_members);
	} else if (p_members.has(StringName(rhs))) {
		return p_members.find(StringName(rhs))->get();
	}
	CharType c0 = rhs[0];
	if ((c0 >= '0' && c0 <= '9') || (c0 == '-' && rhs.length() > 1) || c0 == '.') {
		return rhs.to_double();
	}
	return Variant();
}

/* Struct govdesinden alan adlarini cikarir. */
inline void parse_struct_fields(const String &p_body, Vector<String> &r_fields) {
	Vector<String> parts = split_top_level(p_body, ';');
	for (int i = 0; i < parts.size(); i++) {
		String part = parts[i].strip_edges();
		if (part.empty()) {
			continue;
		}
		Vector<String> words = part.split(" ", false);
		if (words.empty()) {
			continue;
		}
		String field = words[words.size() - 1];
		int b = field.find("[");
		if (b != -1) {
			field = field.substr(0, b);
		}
		if (is_valid_identifier(field)) {
			r_fields.push_back(field);
		}
	}
}

/* ".alan = deger, ..." -> Dictionary. */
inline Dictionary parse_struct_literal(const String &p_body, const Map<StringName, Variant> &p_members) {
	Dictionary d;
	Vector<String> parts = split_top_level(p_body, ',');
	for (int i = 0; i < parts.size(); i++) {
		String part = parts[i].strip_edges();
		while (part.begins_with(".")) {
			part = part.substr(1);
		}
		int eq = part.find("=");
		if (eq == -1) {
			continue;
		}
		String key = part.substr(0, eq).strip_edges();
		String val = part.substr(eq + 1).strip_edges();
		if (is_valid_identifier(key)) {
			d[key] = initialize_value(val, p_members);
		}
	}
	return d;
}

/* "(a, "b", 10)" -> Array. */
inline Array parse_tuple_literal(const String &p_body, const Map<StringName, Variant> &p_members) {
	Array arr;
	Vector<String> parts = split_top_level(p_body, ',');
	for (int i = 0; i < parts.size(); i++) {
		arr.push_back(initialize_value(parts[i], p_members));
	}
	return arr;
}

/* "anahtar : deger, ..." -> Dictionary. */
inline Dictionary parse_dict_literal(const String &p_body, const Map<StringName, Variant> &p_members) {
	Dictionary d;
	Vector<String> parts = split_top_level(p_body, ',');
	for (int i = 0; i < parts.size(); i++) {
		String part = parts[i].strip_edges();
		int col = part.find(":");
		if (col == -1) {
			continue;
		}
		String key = part.substr(0, col).strip_edges();
		String val = part.substr(col + 1).strip_edges();
		Vector<String> kw = key.split(" ", false);
		String kname = kw.empty() ? key : kw[kw.size() - 1];
		if (is_valid_identifier(kname)) {
			d[kname] = initialize_value(val, p_members);
		}
	}
	return d;
}

/* "enum Color { RED, GREEN };" bildirimi. */
inline void handle_enum_stmt(const String &p_stmt, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types) {
	int open = p_stmt.find("{");
	if (open == -1) {
		return;
	}
	int close = p_stmt.rfind("}");
	if (close == -1 || close < open) {
		return;
	}
	String body = p_stmt.substr(open + 1, close - open - 1);
	String head = p_stmt.substr(0, open).strip_edges();
	Vector<String> hw = head.split(" ", false);
	String type_name;
	for (int i = 1; i < hw.size(); i++) {
		if (hw[i] == "struct" || hw[i] == "enum" || hw[i] == "union") {
			continue;
		}
		type_name = hw[i];
		break;
	}

	GCLTypeInfo info;
	info.is_enum = true;
	int val = 0;
	Vector<String> parts = split_top_level(body, ',');
	for (int i = 0; i < parts.size(); i++) {
		String item = parts[i].strip_edges();
		if (item.empty()) {
			continue;
		}
		String name = item;
		int eqi = item.find("=");
		if (eqi != -1) {
			name = item.substr(0, eqi).strip_edges();
			val = item.substr(eqi + 1).strip_edges().to_int();
		}
		if (is_valid_identifier(name)) {
			info.members.push_back(name);
			p_members[StringName(name)] = val;
			val++;
		}
	}
	if (!type_name.empty()) {
		p_types.types[StringName(type_name)] = info;
	}
}

/* "struct Player { ... };" bildirimi (typedef'siz). */
inline void handle_struct_stmt(const String &p_stmt, GCLTypeRegistry &p_types) {
	int open = p_stmt.find("{");
	if (open == -1) {
		return;
	}
	int close = p_stmt.rfind("}");
	if (close == -1 || close < open) {
		return;
	}
	String body = p_stmt.substr(open + 1, close - open - 1);
	String head = p_stmt.substr(0, open).strip_edges();
	Vector<String> hw = head.split(" ", false);
	String type_name;
	for (int i = 1; i < hw.size(); i++) {
		if (hw[i] == "struct" || hw[i] == "union" || hw[i] == "enum") {
			continue;
		}
		type_name = hw[i];
		break;
	}
	if (type_name.empty()) {
		return;
	}
	GCLTypeInfo info;
	info.is_struct = true;
	parse_struct_fields(body, info.members);
	p_types.types[StringName(type_name)] = info;
}

/* Satirdaki # yorumunu keser (basit). */
inline String strip_line_comment(const String &p_line) {
	int hash = p_line.find("#");
	if (hash == -1) {
		return p_line;
	}
	return p_line.substr(0, hash);
}

/* Satirdaki # yorumunu ve #| blok yorumunu keser; blok yorum durumu
   cagrilar arasinda r_in_block ile tasinir. */
inline String strip_line_comment_state(const String &p_line, bool &r_in_block) {
	String out;
	int i = 0;
	const int L = p_line.length();
	while (i < L) {
		if (!r_in_block) {
			if (p_line[i] == '#' && p_line.find("#|", i) == i) {
				r_in_block = true;
				i += 2;
				continue;
			}
			if (p_line[i] == '#') {
				break; /* satir yorumu */
			}
			out += p_line[i];
			i++;
		} else {
			if (p_line.find("|#", i) == i) {
				r_in_block = false;
				i += 2;
				continue;
			}
			i++;
		}
	}
	return out;
}

/* Preprocessor satirlari: #define, #undef, #warning, #error, #debug,
   #ifdef/#ifndef/#if/#elif/#else/#endif (basit bolge yonetimi).
   r_region_active: o anki aktif/pasif bolge bayragi. */
inline bool handle_preprocess(const String &p_line, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, bool &r_region_active) {
	if (!p_line.begins_with("#")) {
		return false;
	}
	String s = p_line.strip_edges();

	if (s.begins_with("#define ")) {
		String rest = s.substr(8).strip_edges();
		int sp = rest.find(" ");
		if (sp != -1) {
			String name = rest.substr(0, sp).strip_edges();
			String val = rest.substr(sp + 1).strip_edges();
			p_members[StringName(name)] = solve_arith(val, p_members);
		} else {
			p_members[StringName(rest)] = true;
		}
		return true;
	}
	if (s.begins_with("#undef ")) {
		String name = s.substr(7).strip_edges();
		if (p_members.has(StringName(name))) {
			p_members.erase(StringName(name));
		}
		return true;
	}
	/* #warning/#error/#debug: "text", "text", değişken, ... çoklu argüman */
	if (s.begins_with("#warning") || s.begins_with("#error") || s.begins_with("#debug")) {
		if (r_region_active) {
			const char *label = s.begins_with("#warning") ? "GCL warning" : (s.begins_with("#error") ? "GCL error" : "GCL debug");
			int head_len = s.begins_with("#warning") ? 8 : (s.begins_with("#error") ? 6 : 6);
			String args = s.substr(head_len).strip_edges();
			String out = label;
			Vector<String> parts = split_top_level(args, ',');
			/* birden fazla "..." arka arkaya veya değişkenler: hepsini birleştir */
			bool first = true;
			for (int i = 0; i < parts.size(); i++) {
				String part = parts[i].strip_edges();
				if (part.empty()) {
					continue;
				}
				if (first) {
					out += ": ";
					first = false;
				} else {
					out += " ";
				}
				if (part.length() >= 2 && part[0] == '"' && part[part.length() - 1] == '"') {
					out += part.substr(1, part.length() - 2);
				} else {
					Variant v = initialize_value(part, p_members);
					out += variant_str(v);
				}
			}
			print_line(out);
		}
		return true;
	}
	if (s == "#ifdef" || s.begins_with("#ifdef ")) {
		String name = s == "#ifdef" ? String() : s.substr(7).strip_edges();
		r_region_active = p_members.has(StringName(name));
		return true;
	}
	if (s == "#ifndef" || s.begins_with("#ifndef ")) {
		String name = s == "#ifndef" ? String() : s.substr(8).strip_edges();
		r_region_active = !p_members.has(StringName(name));
		return true;
	}
	if (s == "#elif" || s.begins_with("#elif ")) {
		/* onceki dal pasifken degerlendir */
		if (!r_region_active) {
			String cond = s == "#elif" ? String() : s.substr(5).strip_edges();
			bool val = false;
			if (cond == "windows") {
				val = true;
			} else if (cond.begins_with("defined(") && cond.ends_with(")")) {
				String name = cond.substr(8, cond.length() - 9).strip_edges();
				val = p_members.has(StringName(name));
			} else {
				Variant cv = solve_arith(cond, p_members);
				val = executor_ops::normalize_solve_bool(cv);
			}
			r_region_active = val;
		}
		return true;
	}
	if (s == "#else") {
		r_region_active = !r_region_active;
		return true;
	}
	if (s == "#endif") {
		r_region_active = true;
		return true;
	}
	if (s == "#if" || s.begins_with("#if ")) {
		/* basit: #if windows / #if linux / #if defined(NAME) */
		String cond = s == "#if" ? String() : s.substr(3).strip_edges();
		bool val = false;
		if (cond == "windows") {
			val = true;
		} else if (cond.begins_with("defined(") && cond.ends_with(")")) {
			String name = cond.substr(8, cond.length() - 9).strip_edges();
			val = p_members.has(StringName(name));
		} else {
			Variant cv = solve_arith(cond, p_members);
			val = executor_ops::normalize_solve_bool(cv);
		}
		r_region_active = val;
		return true;
	}
	return false;
}

/* Fonksiyon adini tam kelime olarak arar. */
inline bool find_call_word(const String &p_line, const String &p_fn, int &r_pos) {
	int pos = 0;
	const int L = p_line.length();
	const int fn_len = p_fn.length();
	while (pos < L) {
		int found = p_line.find(p_fn, pos);
		if (found == -1) {
			return false;
		}
		int end = found + fn_len;
		bool boundary_ok =
				(found == 0 || !is_ident_char(p_line[found - 1])) &&
				(end >= L || !is_ident_char(p_line[end]));
		if (boundary_ok) {
			r_pos = found;
			return true;
		}
		pos = found + fn_len;
	}
	return false;
}

/* PrintF("...", a, b) / print("...") cagrisini isler. */
inline bool handle_call_ex(const String &p_line, const Map<StringName, Variant> &p_members) {
	for (int pass = 0; pass < 2; pass++) {
		const String fn = pass == 0 ? "PrintF" : "print";
		int pi = -1;
		if (!find_call_word(p_line, fn, pi)) {
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
			args.push_back(initialize_value(arg_strings[k], p_members));
		}

		int consumed = 0;
		String out = apply_printf(fmt, args, consumed);
		if (pass == 0) {
			print_string(out);
		} else {
			print_line(out);
		}
		return true;
	}

	return false;
}

/* "typedef" bildirimi: bloklu (struct/enum/union) veya basit. */
inline void handle_typedef_stmt(const String &p_stmt, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types) {
	if (p_stmt.find("struct") != -1 || p_stmt.find("enum") != -1 || p_stmt.find("union") != -1) {
		int open = p_stmt.find("{");
		if (open != -1) {
			int close = p_stmt.rfind("}");
			if (close > open) {
				String body = p_stmt.substr(open + 1, close - open - 1);
				String tail = p_stmt.substr(close + 1).strip_edges().replace(";", "").strip_edges();
				String head = p_stmt.substr(0, open).strip_edges();
				Vector<String> hw = head.split(" ", false);
				String head_name;
				for (int i = hw.size() - 1; i > 0; i--) {
					if (hw[i] == "typedef" || hw[i] == "struct" || hw[i] == "enum" || hw[i] == "union") {
						break;
					}
					head_name = hw[i];
					break;
				}

				bool is_struct_kw = head.find("struct") != -1 || head.find("union") != -1;
				if (is_struct_kw) {
					GCLTypeInfo info;
					info.is_struct = true;
					parse_struct_fields(body, info.members);
					if (!tail.empty()) {
						p_types.types[StringName(tail)] = info;
					}
					if (!head_name.empty()) {
						p_types.types[StringName(head_name)] = info;
					}
				} else {
					String enum_head = "enum " + head_name;
					handle_enum_stmt(enum_head + " {" + body + "}", p_members, p_types);
					if (p_types.types.has(StringName(head_name)) && !tail.empty()) {
						p_types.types[StringName(tail)] = p_types.types[StringName(head_name)];
					}
				}
				return;
			}
		}
	}

	String s = p_stmt.replace(";", "").strip_edges();
	Vector<String> w = s.split(" ", false);
	if (w.size() >= 3 && w[0] == "typedef") {
		String orig;
		for (int i = 1; i < w.size() - 1; i++) {
			if (i > 1) {
				orig += " ";
			}
			orig += w[i];
		}
		String alias = w[w.size() - 1];
		if (is_valid_identifier(alias)) {
			p_types.aliases[StringName(alias)] = orig;
		}
	}
}

} // namespace executor_core

/* executor_core::initialize_value'nun ileri bildirimi (executor_ops.h'de):
   buradaki inline tanim, gcl::executor_ops namespace'indedir. */
namespace executor_ops {

inline Variant initialize_value_fwd(const String &p_rhs, const Map<StringName, Variant> &p_members) {
	return executor_core::initialize_value(p_rhs, p_members);
}

/* executor_ops.h'de bildirilen ops_initialize_value'nun TANIMI. */
inline Variant ops_initialize_value(const String &p_rhs, const Map<StringName, Variant> &p_members) {
	String rhs = p_rhs.strip_edges();
	if (rhs.begins_with("sizeof(") && rhs.ends_with(")")) {
		String inner = rhs.substr(7, rhs.length() - 8).strip_edges();
		return (double)sizeof_type(inner);
	}
	return initialize_value_fwd(rhs, p_members);
}

} // namespace executor_ops

} // namespace gcl

#endif // GCL_EXECUTOR_CORE_H
