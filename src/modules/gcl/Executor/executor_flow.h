/**************************************************************************/
/*  executor_flow.h                                                      */
/**************************************************************************/
/*  GCL Executor - Kontrol akisi yardimcilari.                            */
/*  Aritmetik/atama, kosul degerlendirme, blok yonetimi ve                */
/*  if/else-if/elif/while/do-while/for/switch donguleri burada.           */
/*  Amac: executor.cpp'yi parcalara bolup her dosyayi <800 satir tutmak.  */
/**************************************************************************/

#ifndef GCL_EXECUTOR_FLOW_H
#define GCL_EXECUTOR_FLOW_H

#include "executor_core.h"

namespace gcl {

/* executor_run_ex ileride tanimlanir (executor.cpp).
   Default arguman yalnizca executor.h'daki bildirimde verilir. */
void executor_run_ex(const String &p_body, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit, Variant *p_retval);

namespace executor_flow {

/* "<tip> <param>, <tip> <param>" parametre listesini ayristirir. */
inline Vector<String> parse_func_params(const String &p_params) {
	Vector<String> names;
	Vector<String> parts = executor_core::split_top_level(p_params, ',');
	for (int i = 0; i < parts.size(); i++) {
		String part = parts[i].strip_edges();
		if (part.empty()) {
			continue;
		}
		String t;
		String n;
		if (executor_core::split_declaration(part, t, n) && executor_core::is_valid_identifier(n)) {
			names.push_back(n);
		}
	}
	return names;
}

/* "foo(a, b)" kullanici fonksiyonu cagrisini isler.
   p_types.source icinde "<tip> foo(...)" tanimini bulur, parametreleri
   yeni scope'a baglar, gövdeyi calistirir. Donus degeri r_retval'a yazilir
   (fonksiyon return ile biterse). Basarisizsa false dondurur. */
inline bool resolve_user_call(const String &p_line, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, Variant *r_retval = nullptr) {
	String s = p_line.strip_edges().replace(";", "").strip_edges();
	if (s.empty() || s.begins_with("PrintF") || s.begins_with("print") ||
			s.begins_with("if") || s.begins_with("while") || s.begins_with("for") || s.begins_with("switch")) {
		return false;
	}

	int open = -1;
	int i = 0;
	const int L = s.length();
	while (i < L) {
		CharType c = s[i];
		if (!executor_core::is_ident_char(c) && c != '_') {
			break;
		}
		i++;
	}
	String fname = s.substr(0, i);
	if (fname.empty() || i >= L || s[i] != '(') {
		return false;
	}
	open = i;

	/* kapanis parantez: icerideki dengeli parantezleri say. */
	int depth = 0;
	int close = -1;
	bool in_str = false;
	for (int j = open; j < L; j++) {
		CharType c = s[j];
		if (c == '"') {
			in_str = !in_str;
		} else if (!in_str) {
			if (c == '(') {
				depth++;
			} else if (c == ')') {
				depth--;
				if (depth == 0) {
					close = j;
					break;
				}
			}
		}
	}
	if (close == -1 || close != L - 1) {
		return false;
	}

	/* recursion guard */
	if (p_types.call_depth >= 32) {
		return false;
	}

	String args_region = s.substr(open + 1, close - open - 1);
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

	/* tanimi bul. */
	int def_open_brace = -1;
	if (!find_func_def(p_types.source, fname, def_open_brace)) {
		return false;
	}

	/* imza -> parametre adlari */
	String sig_region;
	{
		int d = p_types.source.find(fname);
		int dp = p_types.source.find("(", d + fname.length());
		int dd = 0;
		int dc = -1;
		for (int j = dp; j < p_types.source.length(); j++) {
			if (p_types.source[j] == '(') {
				dd++;
			} else if (p_types.source[j] == ')') {
				dd--;
				if (dd == 0) {
					dc = j;
					break;
				}
			}
		}
		if (dc != -1) {
			sig_region = p_types.source.substr(dp + 1, dc - dp - 1);
		}
	}
	Vector<String> param_names = parse_func_params(sig_region);

	/* yeni scope: parametre bagla */
	Map<StringName, Variant> call_scope;
	for (int k = 0; k < param_names.size(); k++) {
		Variant v;
		if (k < arg_strings.size()) {
			String arg = arg_strings[k].strip_edges();
			v = executor_core::initialize_value(arg, p_members);
		}
		call_scope[StringName(param_names[k])] = v;
	}

	/* govdeyi kapsamli kopyala */
	String body;
	{
		int bd = 1;
		int j = def_open_brace + 1;
		const int SL = p_types.source.length();
		for (; j < SL; j++) {
			if (p_types.source[j] == '{') {
				bd++;
			} else if (p_types.source[j] == '}') {
				bd--;
				if (bd == 0) {
					body = p_types.source.substr(def_open_brace + 1, j - def_open_brace - 1);
					break;
				}
			}
		}
	}
	if (body.empty()) {
		return false;
	}

	p_types.call_depth++;
	int exit_code = 0;
	Variant retv;
	executor_run_ex(body, call_scope, p_types, exit_code, &retv);
	p_types.call_depth--;

	if (r_retval && exit_code == 3) {
		*r_retval = retv;
	}
	return true;
}

}

namespace executor_flow {

/* Aritmetik cozucu executor_ops.h'da genisletildi:
   + - * / % & | ^ ~ << >> == != < > <= >= && || ternary sizeof ve
   GCL typedef operator adlari (and/or/not/equals/bitAnd/...). */
using executor_ops::solve_arith;

/* "i++", "++i", "i--", "i += 2", "x = 5", "x = y + 1" islemlerini uygular. */
inline void apply_assignment(const String &p_line, Map<StringName, Variant> &p_members) {
	String s = p_line.strip_edges().replace(";", "").strip_edges();
	if (s.empty()) {
		return;
	}

	if (s.ends_with("++") || s.begins_with("++")) {
		String name = s.ends_with("++") ? s.substr(0, s.length() - 2).strip_edges() : s.substr(2).strip_edges();
		if (p_members.has(StringName(name))) {
			p_members[StringName(name)] = executor_core::variant_real(p_members[StringName(name)]) + 1.0;
		}
		return;
	}
	if (s.ends_with("--") || s.begins_with("--")) {
		String name = s.ends_with("--") ? s.substr(0, s.length() - 2).strip_edges() : s.substr(2).strip_edges();
		if (p_members.has(StringName(name))) {
			p_members[StringName(name)] = executor_core::variant_real(p_members[StringName(name)]) - 1.0;
		}
		return;
	}

	int eq = s.find("=");
	if (eq <= 0) {
		return;
	}
	String lhs = s.substr(0, eq).strip_edges();
	String rhs = s.substr(eq + 1).strip_edges();

	/* Bilesik atama: += -= *= /= %= */
	if (lhs.length() > 1) {
		CharType opc = lhs[lhs.length() - 1];
		if (opc == '+' || opc == '-' || opc == '*' || opc == '/' || opc == '%') {
			String base = lhs.substr(0, lhs.length() - 1).strip_edges();
			if (p_members.has(StringName(base))) {
				double cur = executor_core::variant_real(p_members[StringName(base)]);
				double val = executor_core::variant_real(solve_arith(rhs, p_members));
				switch (opc) {
					case '+':
						p_members[StringName(base)] = cur + val;
						break;
					case '-':
						p_members[StringName(base)] = cur - val;
						break;
					case '*':
						p_members[StringName(base)] = cur * val;
						break;
					case '/':
						p_members[StringName(base)] = val == 0.0 ? cur : cur / val;
						break;
					case '%':
						p_members[StringName(base)] = val == 0.0 ? cur : (double)((int64_t)cur % (int64_t)val);
						break;
				}
				return;
			}
		}
	}

	/* duz atama: "x = ifade" */
	if (p_members.has(StringName(lhs))) {
		p_members[StringName(lhs)] = solve_arith(rhs, p_members);
	}
}

/* "a == b", "x > 5", "!y", "a && b", "a || b" kosullarini degerlendirir. */
inline bool eval_condition(const String &p_cond, const Map<StringName, Variant> &p_members) {
	/* GCL typedef operator adlarini C'ye cevir: equals -> ==, and -> && ... */
	String c = executor_ops::replace_operator_words(p_cond.strip_edges());
	if (c.empty()) {
		return false;
	}
	if (c.begins_with("(") && c.ends_with(")")) {
		return eval_condition(c.substr(1, c.length() - 2), p_members);
	}
	if (c.begins_with("!")) {
		return !eval_condition(c.substr(1), p_members);
	}

	int opi = c.find("||");
	if (opi != -1) {
		return eval_condition(c.substr(0, opi), p_members) || eval_condition(c.substr(opi + 2), p_members);
	}
	opi = c.find("&&");
	if (opi != -1) {
		return eval_condition(c.substr(0, opi), p_members) && eval_condition(c.substr(opi + 2), p_members);
	}
	opi = c.find("==");
	if (opi != -1) {
		Variant l = executor_core::initialize_value(c.substr(0, opi), p_members);
		Variant rr = executor_core::initialize_value(c.substr(opi + 2), p_members);
		if (l.get_type() == Variant::STRING || rr.get_type() == Variant::STRING) {
			return executor_core::variant_str(l) == executor_core::variant_str(rr);
		}
		return executor_core::variant_real(l) == executor_core::variant_real(rr);
	}
	opi = c.find("!=");
	if (opi != -1) {
		Variant l = executor_core::initialize_value(c.substr(0, opi), p_members);
		Variant rr = executor_core::initialize_value(c.substr(opi + 2), p_members);
		if (l.get_type() == Variant::STRING || rr.get_type() == Variant::STRING) {
			return executor_core::variant_str(l) != executor_core::variant_str(rr);
		}
		return executor_core::variant_real(l) != executor_core::variant_real(rr);
	}
	opi = c.find(">=");
	if (opi != -1) {
		return executor_core::variant_real(executor_core::initialize_value(c.substr(0, opi), p_members)) >= executor_core::variant_real(executor_core::initialize_value(c.substr(opi + 2), p_members));
	}
	opi = c.find("<=");
	if (opi != -1) {
		return executor_core::variant_real(executor_core::initialize_value(c.substr(0, opi), p_members)) <= executor_core::variant_real(executor_core::initialize_value(c.substr(opi + 2), p_members));
	}
	opi = c.find(">");
	if (opi != -1) {
		return executor_core::variant_real(executor_core::initialize_value(c.substr(0, opi), p_members)) > executor_core::variant_real(executor_core::initialize_value(c.substr(opi + 2), p_members));
	}
	opi = c.find("<");
	if (opi != -1) {
		return executor_core::variant_real(executor_core::initialize_value(c.substr(0, opi), p_members)) < executor_core::variant_real(executor_core::initialize_value(c.substr(opi + 2), p_members));
	}

	/* bitwise & | ^ << >> iceren kosullar: "if (x & 1)" */
	if (c.find("&") != -1 || c.find("|") != -1 || c.find("^") != -1 || c.find("<<") != -1 || c.find(">>") != -1) {
		Variant bv = solve_arith(c, p_members);
		if (bv.get_type() == Variant::BOOL) {
			return bv.booleanize();
		}
		if (bv.get_type() == Variant::NIL) {
			return false;
		}
		return executor_core::variant_real(bv) != 0.0;
	}

	Variant v = executor_core::initialize_value(c, p_members);
	if (v.get_type() == Variant::BOOL) {
		return v.booleanize();
	}
	if (v.get_type() == Variant::NIL) {
		return false;
	}
	return executor_core::variant_real(v) != 0.0;
}

/* p_line_pos'tan baslayan { blogunun kapanisindan hemen sonraki karakter konumu. */
inline int find_block_end(const String &p_body, int p_line_pos) {
	const int len = p_body.length();
	int line_start = p_line_pos;
	int depth = 0;
	while (line_start <= len) {
		int line_end = p_body.find("\n", line_start);
		if (line_end == -1) {
			line_end = len;
		}
		String line = p_body.substr(line_start, line_end - line_start);
		for (int i = 0; i < line.length(); i++) {
			if (line[i] == '{') {
				depth++;
			} else if (line[i] == '}') {
				depth--;
				if (depth == 0) {
					return line_start + i + 1;
				}
			}
		}
		if (line_end == len) {
			return len + 1;
		}
		line_start = line_end + 1;
	}
	return len + 1;
}

/* Blok govdesini parantezsiz cikarir: "{ icerik }" -> " icerik ". */
inline void extract_block_body(const String &p_body, int p_start, int p_after, String &r_out) {
	int open = p_body.find("{", p_start);
	if (open == -1 || open >= p_after) {
		r_out = "";
		return;
	}
	int close = p_body.rfind("}", p_after - 1);
	if (close == -1 || close < open) {
		r_out = "";
		return;
	}
	r_out = p_body.substr(open + 1, close - open - 1);
}

/* if / else if / elif / else zincirini isler. Zincirde ILK TRUE dal calisir,
   sonraki else/elif dallari atlanir (C semantigi). r_pos kapanis sonrasina tasinir. */
inline void handle_if_chain(const String &p_body, int &r_pos, const String &p_stmt,
		Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit) {
	String stmt = p_stmt.strip_edges();
	int op = stmt.find("(");
	int cp = stmt.rfind(")");
	if (op == -1 || cp == -1 || cp < op) {
		return;
	}
	String cond = stmt.substr(op + 1, cp - op - 1);

	int current_pos = r_pos;
	bool done_exec = false; /* dal calistiysa (true branch veya else) daha fazla yok */
	bool matched = eval_condition(cond, p_members);
	bool chain_done = false;
	int guard = 0;

	while (!chain_done && current_pos <= p_body.length()) {
		int body_after = find_block_end(p_body, current_pos);
		String inner;
		extract_block_body(p_body, current_pos, body_after, inner);
		if (matched && !done_exec) {
			int inner_exit = 0;
			executor_run_ex(inner, p_members, p_types, inner_exit);
			if (inner_exit != 0) {
				r_exit = inner_exit;
				r_pos = body_after;
				return;
			}
			done_exec = true;
		}
		matched = false;

		current_pos = body_after;
		if (current_pos > p_body.length()) {
			break;
		}

		int line_end = p_body.find("\n", current_pos);
		if (line_end == -1) {
			line_end = p_body.length();
		}
		String next_line = p_body.substr(current_pos, line_end - current_pos).strip_edges();

		if (next_line.begins_with("else") || next_line.begins_with("elif")) {
			bool is_else_if = next_line.begins_with("elif") ||
					(next_line.find("if") != -1 && next_line.find("if") > 1);
			if (is_else_if) {
				int eo = next_line.find("(");
				int ec = next_line.rfind(")");
				String econd = (eo != -1 && ec != -1) ? next_line.substr(eo + 1, ec - eo - 1) : "";
				int else_after = find_block_end(p_body, current_pos);
				if (!done_exec && eval_condition(econd, p_members)) {
					String ebody;
					extract_block_body(p_body, current_pos, else_after, ebody);
					int inner_exit = 0;
					executor_run_ex(ebody, p_members, p_types, inner_exit);
					if (inner_exit != 0) {
						r_exit = inner_exit;
						r_pos = else_after;
						return;
					}
					done_exec = true;
				}
				current_pos = else_after;
			} else {
				int else_after = find_block_end(p_body, current_pos);
				if (!done_exec) {
					String ebody;
					extract_block_body(p_body, current_pos, else_after, ebody);
					int inner_exit = 0;
					executor_run_ex(ebody, p_members, p_types, inner_exit);
					if (inner_exit != 0) {
						r_exit = inner_exit;
						r_pos = else_after;
						return;
					}
					done_exec = true;
				}
				current_pos = else_after;
				chain_done = true;
			}
		} else {
			chain_done = true;
		}

		guard++;
		if (guard > 50) {
			chain_done = true;
		}
	}

	r_pos = current_pos;
}

/* while (kosul) { ... } dongusu. */
inline void run_while(const String &p_body, int &r_pos, const String &p_stmt,
		Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit) {
	String stmt = p_stmt.strip_edges();
	int op = stmt.find("(");
	int cp = stmt.rfind(")");
	if (op == -1 || cp == -1 || cp < op) {
		return;
	}
	String cond = stmt.substr(op + 1, cp - op - 1);

	int body_after = find_block_end(p_body, r_pos);
	int guard = 0;
	while (eval_condition(cond, p_members)) {
		String inner;
		extract_block_body(p_body, r_pos, body_after, inner);
		int inner_exit = 0;
		executor_run_ex(inner, p_members, p_types, inner_exit);
		if (inner_exit == 3) { /* return */
			r_exit = 3;
			r_pos = body_after;
			return;
		}
		if (inner_exit == 1) { /* break */
			break;
		}
		guard++;
		if (guard > 1000000) {
			break;
		}
	}
	r_pos = body_after;
}

/* do { ... } while (kosul); dongusu. */
inline void run_do_while(const String &p_body, int &r_pos,
		Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit) {
	int body_after = find_block_end(p_body, r_pos);
	if (body_after > p_body.length()) {
		return;
	}

	int guard = 0;
	for (;;) {
		String inner;
		extract_block_body(p_body, r_pos, body_after, inner);
		int inner_exit = 0;
		executor_run_ex(inner, p_members, p_types, inner_exit);
		if (inner_exit == 3) {
			r_exit = 3;
			r_pos = body_after;
			return;
		}
		if (inner_exit == 1) {
			break;
		}

		int line_end = p_body.find("\n", body_after);
		if (line_end == -1) {
			line_end = p_body.length();
		}
		String wline = p_body.substr(body_after, line_end - body_after).strip_edges();
		int op = wline.find("(");
		int cp = wline.rfind(")");
		if (op == -1 || cp == -1) {
			break;
		}
		String cond = wline.substr(op + 1, cp - op - 1);
		if (!eval_condition(cond, p_members)) {
			break;
		}

		guard++;
		if (guard > 1000000) {
			break;
		}
	}

	/* "} while (kosul);" satiri tuket ki while sanilmasin */
	r_pos = body_after;
	{
		int wl_end = p_body.find("\n", r_pos);
		if (wl_end == -1) {
			wl_end = p_body.length();
		}
		String wl = p_body.substr(r_pos, wl_end - r_pos).strip_edges();
		if (wl.begins_with("while")) {
			r_pos = wl_end + 1;
		}
	}
}

/* for : C-tarzi "for(int i = 0; i < n; i++)" veya foreach "for (i in arr)". */
inline void run_for(const String &p_body, int &r_pos, const String &p_stmt,
		Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit) {
	String stmt = p_stmt.strip_edges();
	int op = stmt.find("(");
	int cp = stmt.rfind(")");
	if (op == -1 || cp == -1 || cp < op) {
		return;
	}
	String inner = stmt.substr(op + 1, cp - op - 1);

	int body_after = find_block_end(p_body, r_pos);
	if (body_after > p_body.length()) {
		return;
	}

	int in_idx = inner.find(" in ");
	if (in_idx != -1) {
		/* foreach */
		String var = inner.substr(0, in_idx).strip_edges();
		String src = inner.substr(in_idx + 4).strip_edges();
		Variant container = executor_core::evaluate_expr(src, p_members);
		if (container.get_type() == Variant::ARRAY) {
			Array arr = container;
			int guard = 0;
			for (int i = 0; i < arr.size(); i++) {
				p_members[StringName(var)] = arr[i];
				String bbody;
				extract_block_body(p_body, r_pos, body_after, bbody);
				int inner_exit = 0;
				executor_run_ex(bbody, p_members, p_types, inner_exit);
				if (inner_exit == 3) {
					r_exit = 3;
					break;
				}
				if (inner_exit == 1) {
					break;
				}
				guard++;
				if (guard > 1000000) {
					break;
				}
			}
		}
		r_pos = body_after;
		return;
	}

	Vector<String> parts = executor_core::split_top_level(inner, ';');
	if (parts.size() < 3) {
		return;
	}
	String init_s = parts[0].strip_edges();
	String cond_s = parts[1].strip_edges();
	String incr_s = parts[2].strip_edges();

	if (!init_s.empty()) {
		int eq = init_s.find("=");
		if (eq != -1) {
			String lhs = init_s.substr(0, eq).strip_edges();
			String rhs = init_s.substr(eq + 1).strip_edges();
			String type;
			String name;
			if (executor_core::split_declaration(lhs, type, name) && executor_core::is_ident_start(name[0])) {
				p_members[StringName(name)] = solve_arith(rhs, p_members);
			} else if (p_members.has(StringName(lhs))) {
				p_members[StringName(lhs)] = solve_arith(rhs, p_members);
			}
		} else {
			apply_assignment(init_s, p_members);
		}
	}

	int guard = 0;
	while (eval_condition(cond_s, p_members)) {
		String bbody;
		extract_block_body(p_body, r_pos, body_after, bbody);
		int inner_exit = 0;
		executor_run_ex(bbody, p_members, p_types, inner_exit);
		if (inner_exit == 3) {
			r_exit = 3;
			break;
		}
		if (inner_exit == 1) { /* break */
			break;
		}
		if (!incr_s.empty()) {
			apply_assignment(incr_s, p_members);
		}
		guard++;
		if (guard > 1000000) {
			break;
		}
	}
	r_pos = body_after;
}

/* switch (ifade) { case X: ... break; default: ... } isler. */
inline int run_switch(const String &p_body, int &r_pos, const String &p_stmt,
		Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types) {
	String stmt = p_stmt.strip_edges();
	int op = stmt.find("(");
	int cp = stmt.rfind(")");
	if (op == -1 || cp == -1 || cp < op) {
		return 0;
	}
	String sw_expr = stmt.substr(op + 1, cp - op - 1);
	Variant sw_val = executor_core::initialize_value(sw_expr, p_members);

	int body_after = find_block_end(p_body, r_pos);
	String inner;
	extract_block_body(p_body, r_pos, body_after, inner);
	if (inner.empty()) {
		r_pos = body_after;
		return 0;
	}

	bool matched = false;
	bool in_default = false;
	int line_start = 0;
	const int len = inner.length();
	while (line_start <= len) {
		int line_end = inner.find("\n", line_start);
		if (line_end == -1) {
			line_end = len;
		}
		String line = inner.substr(line_start, line_end - line_start).strip_edges();
		if (!line.empty()) {
			if (line.begins_with("case ")) {
				if (matched) {
					break;
				}
				String label = line.substr(5).replace(":", "").strip_edges();
				in_default = false;
				if (executor_core::variant_str(sw_val) == executor_core::variant_str(executor_core::initialize_value(label, p_members)) ||
						executor_core::variant_real(sw_val) == executor_core::variant_real(executor_core::initialize_value(label, p_members))) {
					matched = true;
				}
			} else if (line == "default") {
				if (matched) {
					break;
				}
				in_default = true;
				matched = true;
			} else if (line == "break" && (matched || in_default)) {
				/* case govdesi bitti */
				if (matched) {
					matched = false;
					break;
				}
			} else if (matched || in_default) {
				int inner_exit = 0;
				String single = line;
				/* tek satir deyimleri olabildigince dogrudan isle: atama / PrintF */
				int eq = single.find("=");
				if (eq > 0 && !single.begins_with("PrintF") && !single.begins_with("print")) {
					String lhs = single.substr(0, eq).strip_edges();
					String rhs = single.substr(eq + 1).replace(";", "").strip_edges();
					if (p_members.has(StringName(lhs))) {
						p_members[StringName(lhs)] = solve_arith(rhs, p_members);
					}
				} else if (single.begins_with("PrintF") || single.begins_with("print")) {
					executor_core::handle_call_ex(single, p_members);
				} else {
					executor_run_ex(single + "\n", p_members, p_types, inner_exit);
				}
				if (inner_exit != 0) {
					r_pos = body_after;
					return inner_exit;
				}
			}
		}
		if (line_end == len) {
			break;
		}
		line_start = line_end + 1;
	}

	r_pos = body_after;
	return 0;
}

} // namespace executor_flow

} // namespace gcl

#endif // GCL_EXECUTOR_FLOW_H
