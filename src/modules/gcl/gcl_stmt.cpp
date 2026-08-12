/**************************************************************************/
/*  gcl_stmt.cpp                                                          */
/**************************************************************************/
/*  Statement executor: if/elif/else, while, for, return, break,           */
/*  continue, printf, and variable/member assignment.                     */
/**************************************************************************/

#include "gcl_core.h"
#include "gcl_script.h"

static bool line_has_else_keyword(const String &p_line) {
	String l = p_line.strip_edges();
	int pos = l.find("else");
	while (pos != -1) {
		bool prev_ok = pos == 0 || !(l[pos - 1] == '_' || (l[pos - 1] >= 'a' && l[pos - 1] <= 'z') || (l[pos - 1] >= 'A' && l[pos - 1] <= 'Z') || (l[pos - 1] >= '0' && l[pos - 1] <= '9'));
		int after = pos + 4;
		bool next_ok = after >= l.length() || !(l[after] == '_' || (l[after] >= 'a' && l[after] <= 'z') || (l[after] >= 'A' && l[after] <= 'Z') || (l[after] >= '0' && l[after] <= '9'));
		if (prev_ok && next_ok) {
			return true;
		}
		pos = l.find("else", after);
	}
	pos = l.find("elif");
	while (pos != -1) {
		bool prev_ok = pos == 0 || !(l[pos - 1] == '_' || (l[pos - 1] >= 'a' && l[pos - 1] <= 'z') || (l[pos - 1] >= 'A' && l[pos - 1] <= 'Z') || (l[pos - 1] >= '0' && l[pos - 1] <= '9'));
		int after = pos + 4;
		bool next_ok = after >= l.length() || !(l[after] == '_' || (l[after] >= 'a' && l[after] <= 'z') || (l[after] >= 'A' && l[after] <= 'Z') || (l[after] >= '0' && l[after] <= '9'));
		if (prev_ok && next_ok) {
			return true;
		}
		pos = l.find("elif", after);
	}
	return false;
}

static int find_block_end(const Vector<String> &p_lines, int p_start) {
	int count = 0;
	for (int i = p_start; i < p_lines.size(); i++) {
		String l = p_lines[i];
		bool in_str = false;
		bool in_char = false;
		for (int c = 0; c < l.length(); c++) {
			CharType ch = l[c];
			if (ch == '"') {
				in_str = !in_str;
			} else if (ch == '\'') {
				in_char = !in_char;
			} else if (!in_str && !in_char) {
				if (ch == '{') {
					count++;
				} else if (ch == '}') {
					count--;
					if (count <= 0) {
						return i;
					}
				}
			}
		}
		if (count <= 0) {
			return i;
		}
	}
	return -1;
}

struct GCLElseBlock {
	bool has_else;
	int body_start;
	int end;

	GCLElseBlock() {
		has_else = false;
		body_start = -1;
		end = -1;
	}
};

static GCLElseBlock find_else_block(const Vector<String> &p_lines, int p_if_end) {
	GCLElseBlock eb;
	int else_key_line = -1;
	String be = p_lines[p_if_end].strip_edges();
	if (line_has_else_keyword(be)) {
		else_key_line = p_if_end;
	} else if (p_if_end + 1 < p_lines.size()) {
		String nxt = p_lines[p_if_end + 1].strip_edges();
		if (nxt.begins_with("else") || nxt.begins_with("elif")) {
			else_key_line = p_if_end + 1;
		}
	}
	if (else_key_line == -1) {
		return eb;
	}
	int brace_line = else_key_line;
	int brace = p_lines[brace_line].find("{");
	if (brace == -1) {
		brace_line = else_key_line + 1;
		if (brace_line < p_lines.size()) {
			brace = p_lines[brace_line].find("{");
		}
	}
	if (brace == -1) {
		return eb;
	}
	int count = 0;
	bool started = false;
	int end = -1;
	for (int li = brace_line; li < p_lines.size(); li++) {
		String l = p_lines[li];
		int from = (li == brace_line) ? brace : 0;
		bool in_str = false;
		bool in_char = false;
		for (int ci = from; ci < l.length(); ci++) {
			CharType ch = l[ci];
			if (ch == '"') {
				in_str = !in_str;
			} else if (ch == '\'') {
				in_char = !in_char;
			} else if (!in_str && !in_char) {
				if (ch == '{') {
					count++;
					started = true;
				} else if (ch == '}' && started) {
					count--;
				}
			}
		}
		if (started && count <= 0) {
			end = li;
			break;
		}
	}
	if (end == -1) {
		return eb;
	}
	eb.has_else = true;
	eb.body_start = brace_line + 1;
	eb.end = end;
	return eb;
}

/* switch (...) { case X: ... break; default: ... }  (gcl_language.md Akis Kontrolu) */
static int find_switch_end(const Vector<String> &p_lines, int p_start) {
	return find_block_end(p_lines, p_start);
}

static String gcl_extract_between(const String &p_line, CharType p_open, CharType p_close) {
	int open = p_line.find(String::chr(p_open));
	if (open == -1) {
		return String();
	}
	int close = p_line.find(String::chr(p_close), open + 1);
	if (close == -1) {
		return String();
	}
	return p_line.substr(open + 1, close - open - 1);
}

/* switch case degeri eslesmesi: sayi tipleri normalize edilir,
 * string/char direkt karsilastirilir. */
static bool gcl_switch_matches(const Variant &p_a, const Variant &p_b) {
	Variant::Type ta = p_a.get_type();
	Variant::Type tb = p_b.get_type();
	bool a_num = (ta == Variant::INT || ta == Variant::REAL);
	bool b_num = (tb == Variant::INT || tb == Variant::REAL);
	if (a_num && b_num) {
		return (double)p_a == (double)p_b;
	}
	if (ta == Variant::STRING && tb == Variant::STRING) {
		return (String)p_a == (String)p_b;
	}
	if (ta == Variant::BOOL && tb == Variant::BOOL) {
		return (bool)p_a == (bool)p_b;
	}
	return false;
}
static bool truthy(const Variant &p_v) {
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

static void set_err(String *r_error, const String &p_msg, int p_line) {
	if (r_error && *r_error == "") {
		*r_error = p_msg + " (line " + itos(p_line) + ")";
	}
}

static void exec_for_step(const String &p_step, Map<StringName, Variant> &p_locals, GCLScriptInstance *p_inst, int p_line, String *r_error) {
	String st = p_step.strip_edges();
	if (st.ends_with("++")) {
		String vn = st.substr(0, st.length() - 2).strip_edges();
		Variant cur = p_locals[StringName(vn)];
		if (cur.get_type() == Variant::INT) {
			p_locals[StringName(vn)] = (int)cur + 1;
		} else if (cur.get_type() == Variant::REAL) {
			p_locals[StringName(vn)] = (float)cur + 1.0f;
		}
	} else if (st.ends_with("--")) {
		String vn = st.substr(0, st.length() - 2).strip_edges();
		Variant cur = p_locals[StringName(vn)];
		if (cur.get_type() == Variant::INT) {
			p_locals[StringName(vn)] = (int)cur - 1;
		} else if (cur.get_type() == Variant::REAL) {
			p_locals[StringName(vn)] = (float)cur - 1.0f;
		}
	} else {
		int eqp = st.find("+=");
		if (eqp != -1) {
			String vn = st.substr(0, eqp).strip_edges();
			String rhs = st.substr(eqp + 2).strip_edges();
			Variant cur = p_locals[StringName(vn)];
			Variant inc = gcl_eval_expr(rhs, p_locals, p_inst, p_line, r_error);
			if (cur.get_type() == Variant::INT && inc.get_type() == Variant::INT) {
				p_locals[StringName(vn)] = (int)cur + (int)inc;
			} else {
				p_locals[StringName(vn)] = (float)((double)cur) + (float)((double)inc);
			}
		}
	}
}

GCLExecResult gcl_exec_body(const Vector<String> &p_lines, int p_start, int p_end, Map<StringName, Variant> &p_locals, GCLScriptInstance *p_inst, int p_base_line, String *r_error) {
	GCLExecResult res;
	int i = p_start;
	while (i < p_end) {
		String line = gcl_strip_inline_comment(p_lines[i]).strip_edges();
		int cline = p_base_line + i;
		if (line == "") {
			i++;
			continue;
		}
		// printf("fmt", ...);
		if (line.begins_with("printf(") && line.ends_with(");")) {
			String inside = line.substr(7, line.length() - 9);
			Vector<String> parts = gcl_split_top_commas(inside);
			Vector<Variant> args;
			if (parts.size() > 0) {
				String fmt = parts[0].strip_edges();
				if (fmt.length() >= 2 && fmt[0] == '"' && fmt[fmt.length() - 1] == '"') {
					fmt = fmt.substr(1, fmt.length() - 2);
				}
				for (int k = 1; k < parts.size(); k++) {
					args.push_back(gcl_eval_expr(parts[k], p_locals, p_inst, cline, r_error));
				}
				gcl_printf_output(gcl_printf_format(fmt, args));
			}
			i++;
			continue;
		}
		// switch (...)
		if (line.begins_with("switch ") || line.begins_with("switch(")) {
			String s_cond = gcl_extract_between(line, '(', ')');
			Variant s_val = gcl_eval_expr(s_cond, p_locals, p_inst, cline, r_error);
			int s_end = find_switch_end(p_lines, i);
			if (s_end == -1) {
				set_err(r_error, "unbalanced braces in switch", cline);
				break;
			}
			bool matched = false;
			bool found_default = false;
			int run_from = -1;
			String default_end_line = "";
			for (int si = i + 1; si < s_end; si++) {
				String s_line = gcl_strip_inline_comment(p_lines[si]).strip_edges();
				if (s_line.begins_with("case ") && s_line.ends_with(":")) {
					String cval = s_line.substr(5, s_line.length() - 6).strip_edges();
					Variant cv = gcl_eval_expr(cval, p_locals, p_inst, cline, r_error);
					if (matched) {
						continue;
					}
					if (gcl_switch_matches(s_val, cv)) {
						matched = true;
						run_from = si + 1;
					}
				} else if (s_line.begins_with("default:")) {
					found_default = true;
					default_end_line = s_line;
					if (!matched) {
						matched = true;
						run_from = si + 1;
					}
				}
			}
			if (matched) {
				// case bloklarini atlayan: body'yi calistir, break'te dur
				GCLExecResult sub = gcl_exec_body(p_lines, run_from, s_end, p_locals, p_inst, p_base_line, r_error);
				if (sub.ret) {
					res.ret = true;
					res.ret_value = sub.ret_value;
					i = p_end;
				} else if (sub.break_loop) {
					// switch icindeki break: switch'ten cik, donguyu kirma
					res.break_loop = false;
					i = s_end + 1;
				} else if (sub.continue_loop) {
					res.continue_loop = true;
					i = p_end;
				} else {
					i = s_end + 1;
				}
			} else {
				i = s_end + 1;
			}
			continue;
		}
		// return / break / continue
		if (line.begins_with("return")) {
			String rest = line.substr(6).strip_edges();
			if (rest.ends_with(";")) {
				rest = rest.substr(0, rest.length() - 1);
			}
			res.ret = true;
			res.ret_value = gcl_eval_expr(rest, p_locals, p_inst, cline, r_error);
			break;
		}
		if (line == "break;" || line == "break") {
			res.break_loop = true;
			break;
		}
		if (line == "continue;" || line == "continue") {
			res.continue_loop = true;
			break;
		}
		// if
		if (line.begins_with("if ") || line.begins_with("if(")) {
			int open = line.find("(");
			int close = line.find(")");
			if (open == -1 || close == -1 || close < open) {
				set_err(r_error, "malformed if statement", cline);
				break;
			}
			String cond = line.substr(open + 1, close - open - 1);
			int block_end = find_block_end(p_lines, i);
			if (block_end == -1) {
				set_err(r_error, "unbalanced braces in if", cline);
				break;
			}
			if (truthy(gcl_eval_expr(cond, p_locals, p_inst, cline, r_error))) {
				GCLExecResult sub = gcl_exec_body(p_lines, i + 1, block_end, p_locals, p_inst, p_base_line, r_error);
				if (sub.ret) {
					res.ret = true;
					res.ret_value = sub.ret_value;
					break;
				}
				if (sub.break_loop) {
					res.break_loop = true;
					break;
				}
				if (sub.continue_loop) {
					res.continue_loop = true;
					break;
				}
			} else {
				GCLElseBlock eb = find_else_block(p_lines, block_end);
				while (eb.has_else) {
					String else_line = "";
					int cond_start = eb.body_start - 1;
					if (cond_start >= 0) {
						else_line = gcl_strip_inline_comment(p_lines[cond_start]).strip_edges();
					}
					if (else_line.begins_with("elif") || else_line.begins_with("else if")) {
						int eo = else_line.find("(");
						int ec = else_line.find(")");
						if (eo != -1 && ec != -1 && ec > eo) {
							String econd = else_line.substr(eo + 1, ec - eo - 1);
							if (truthy(gcl_eval_expr(econd, p_locals, p_inst, cline, r_error))) {
								GCLExecResult sub = gcl_exec_body(p_lines, eb.body_start, eb.end, p_locals, p_inst, p_base_line, r_error);
								if (sub.ret) {
									res.ret = true;
									res.ret_value = sub.ret_value;
									i = p_end;
								} else if (sub.break_loop) {
									res.break_loop = true;
									i = p_end;
								} else if (sub.continue_loop) {
									res.continue_loop = true;
									i = p_end;
								}
								eb.has_else = false;
								break;
							}
						}
						eb = find_else_block(p_lines, eb.end);
					} else {
						GCLExecResult sub = gcl_exec_body(p_lines, eb.body_start, eb.end, p_locals, p_inst, p_base_line, r_error);
						if (sub.ret) {
							res.ret = true;
							res.ret_value = sub.ret_value;
							i = p_end;
						} else if (sub.break_loop) {
							res.break_loop = true;
							i = p_end;
						} else if (sub.continue_loop) {
							res.continue_loop = true;
							i = p_end;
						}
						break;
					}
				}
			}
			i = block_end + 1;
			continue;
		}
		// while
		if (line.begins_with("while ") || line.begins_with("while(")) {
			int open = line.find("(");
			int close = line.find(")");
			if (open == -1 || close == -1 || close < open) {
				set_err(r_error, "malformed while statement", cline);
				break;
			}
			String cond = line.substr(open + 1, close - open - 1);
			int block_end = find_block_end(p_lines, i);
			if (block_end == -1) {
				set_err(r_error, "unbalanced braces in while", cline);
				break;
			}
			while (truthy(gcl_eval_expr(cond, p_locals, p_inst, cline, r_error))) {
				GCLExecResult sub = gcl_exec_body(p_lines, i + 1, block_end, p_locals, p_inst, p_base_line, r_error);
				if (sub.ret) {
					res.ret = true;
					res.ret_value = sub.ret_value;
					break;
				}
				if (sub.break_loop) {
					break;
				}
			}
			i = block_end + 1;
			continue;
		}
		// do { } while (...)
		if (line.begins_with("do") && (line == "do" || line.begins_with("do{"))) {
			int do_end = find_block_end(p_lines, i);
			if (do_end == -1) {
				set_err(r_error, "unbalanced braces in do", cline);
				break;
			}
			// while (...);  satiri ara
			String w_cond;
			bool have_while = false;
			int while_line_idx = -1;
			for (int wi = do_end + 1; wi < p_lines.size(); wi++) {
				String w_line = gcl_strip_inline_comment(p_lines[wi]).strip_edges();
				if (w_line == "") {
					continue;
				}
				if (w_line.begins_with("while")) {
					String cond = gcl_extract_between(w_line, '(', ')');
					if (cond != "") {
						w_cond = cond;
						have_while = true;
						while_line_idx = wi;
					}
				}
				break;
			}
			GCLExecResult sub = gcl_exec_body(p_lines, i + 1, do_end, p_locals, p_inst, p_base_line, r_error);
			if (sub.ret) {
				res.ret = true;
				res.ret_value = sub.ret_value;
				break;
			}
			if (sub.break_loop) {
				i = (while_line_idx != -1) ? while_line_idx + 1 : do_end + 1;
				continue; // do-while'dan cik
			}
			while (have_while && truthy(gcl_eval_expr(w_cond, p_locals, p_inst, cline, r_error))) {
				GCLExecResult sub2 = gcl_exec_body(p_lines, i + 1, do_end, p_locals, p_inst, p_base_line, r_error);
				if (sub2.ret) {
					res.ret = true;
					res.ret_value = sub2.ret_value;
					break;
				}
				if (sub2.break_loop) {
					break;
				}
			}
			i = (while_line_idx != -1) ? while_line_idx + 1 : do_end + 1;
			continue;
		}
		// for
		if (line.begins_with("for ") || line.begins_with("for(")) {
			int open = line.find("(");
			int close = line.find(")");
			if (open == -1 || close == -1 || close < open) {
				set_err(r_error, "malformed for statement", cline);
				break;
			}
			String header = line.substr(open + 1, close - open - 1);
			int block_end = find_block_end(p_lines, i);
			if (block_end == -1) {
				set_err(r_error, "unbalanced braces in for", cline);
				break;
			}
			// for (i in REF.Childs)
			int in_pos = header.find(" in ");
			if (in_pos != -1) {
				String var_name = header.substr(0, in_pos).strip_edges();
				String iter_expr = header.substr(in_pos + 4).strip_edges();
				Variant iterable = gcl_eval_expr(iter_expr, p_locals, p_inst, cline, r_error);
				if (iterable.get_type() == Variant::ARRAY) {
					Array arr = iterable;
					for (int k = 0; k < arr.size(); k++) {
						p_locals[StringName(var_name)] = arr[k];
						GCLExecResult sub = gcl_exec_body(p_lines, i + 1, block_end, p_locals, p_inst, p_base_line, r_error);
						if (sub.ret) {
							res.ret = true;
							res.ret_value = sub.ret_value;
							break;
						}
						if (sub.break_loop) {
							break;
						}
					}
				}
			} else {
				Vector<String> parts = header.split(";");
				if (parts.size() == 3) {
					String init = parts[0].strip_edges();
					String cond = parts[1].strip_edges();
					String step = parts[2].strip_edges();
					if (init != "") {
						int eq = init.find("=");
						if (eq != -1) {
							String lhs = init.substr(0, eq).strip_edges();
							Vector<String> lhs_words = lhs.split(" ");
							String var_name = lhs_words[lhs_words.size() - 1].strip_edges();
							String rhs = init.substr(eq + 1).strip_edges();
							p_locals[StringName(var_name)] = gcl_eval_expr(rhs, p_locals, p_inst, cline, r_error);
						}
					}
					while (truthy(gcl_eval_expr(cond, p_locals, p_inst, cline, r_error))) {
						GCLExecResult sub = gcl_exec_body(p_lines, i + 1, block_end, p_locals, p_inst, p_base_line, r_error);
						if (sub.ret) {
							res.ret = true;
							res.ret_value = sub.ret_value;
							break;
						}
						if (sub.break_loop) {
							break;
						}
						exec_for_step(step, p_locals, p_inst, cline, r_error);
					}
				}
			}
			i = block_end + 1;
			continue;
		}
			{
			String aline = line.ends_with(";") ? line.substr(0, line.length() - 1) : line;
			String avar;
			String aop;
			String arhs;
			if (aline.ends_with("++")) {
			avar = aline.substr(0, aline.length() - 2).strip_edges();
			aop = "+=";
			arhs = "1";
			} else if (aline.ends_with("--")) {
			avar = aline.substr(0, aline.length() - 2).strip_edges();
			aop = "-=";
			arhs = "1";
			} else {
			int apos = aline.find("+=");
			if (apos == -1) apos = aline.find("-=");
			if (apos == -1) apos = aline.find("*=");
			if (apos == -1) apos = aline.find("/=");
			if (apos == -1) apos = aline.find("%=");
			if (apos != -1) {
			avar = aline.substr(0, apos).strip_edges();
			aop = aline.substr(apos, 2);
			arhs = aline.substr(apos + 2).strip_edges();
			}
			}
			if (avar != "" && aop != "") {
			Variant cur;
			String target = avar;
			bool is_member = avar.find(".") != -1;
			if (is_member) {
			cur = gcl_eval_expr(avar, p_locals, p_inst, cline, r_error);
			} else {
			Vector<String> lhs_words = avar.split(" ");
			target = lhs_words[lhs_words.size() - 1].strip_edges();
			cur = p_locals[StringName(target)];
			}
			Variant rhs = gcl_eval_expr(arhs, p_locals, p_inst, cline, r_error);
			Variant nv;
			bool a_int = cur.get_type() == Variant::INT;
			bool b_int = rhs.get_type() == Variant::INT;
			if (aop == "+=") {
			nv = (a_int && b_int) ? Variant((int)cur + (int)rhs) : Variant((double)cur + (double)rhs);
			} else if (aop == "-=") {
			nv = (a_int && b_int) ? Variant((int)cur - (int)rhs) : Variant((double)cur - (double)rhs);
			} else if (aop == "*=") {
			nv = (a_int && b_int) ? Variant((int)cur * (int)rhs) : Variant((double)cur * (double)rhs);
			} else if (aop == "/=") {
			nv = (a_int && b_int && (int)rhs != 0) ? Variant((int)cur / (int)rhs) : Variant((double)cur / (double)rhs);
			} else if (aop == "%=") {
			nv = (a_int && b_int && (int)rhs != 0) ? Variant((int)cur % (int)rhs) : Variant((int)((int)cur % (int)rhs));
			}
			if (!is_member) {
			p_locals[StringName(target)] = nv;
			} else {
			gcl_write_member(p_inst, avar, nv, p_locals, cline, r_error);
			}
			i++;
			continue;
			}
			}
		// assignment
		int eq = line.find("=");
		if (eq != -1) {
			String lhs = line.substr(0, eq).strip_edges();
			String rhs = line.substr(eq + 1);
			if (rhs.ends_with(";")) {
				rhs = rhs.substr(0, rhs.length() - 1);
			}
			Vector<String> lhs_words = lhs.split(" ");
			String target = lhs_words[lhs_words.size() - 1].strip_edges();
			if (target.find(".") != -1) {
				Variant val = gcl_eval_expr(rhs, p_locals, p_inst, cline, r_error);
				gcl_write_member(p_inst, target, val, p_locals, cline, r_error);
			} else {
				p_locals[StringName(target)] = gcl_eval_expr(rhs, p_locals, p_inst, cline, r_error);
			}
			i++;
			continue;
		}
		// standalone expression: "print_this_global();" / "Input.IsDown('e')"
		{
			String call_line = line.ends_with(";") ? line.substr(0, line.length() - 1) : line;
			int open = call_line.find("(");
			if (open != -1 && call_line.ends_with(")")) {
				String fname = call_line.substr(0, open).strip_edges();
				// GCL 101: file-scope function call (print_this_global();)
				// or a non-member call "Name(args)".
				if (fname != "" && fname.find(" ") == -1 && fname.find(".") == -1 && fname.find("[") == -1) {
					String inside = call_line.substr(open + 1, call_line.length() - open - 2);
					Vector<String> parts = gcl_split_top_commas(inside);
					Vector<Variant> args;
					bool args_ok = true;
					for (int k = 0; k < parts.size(); k++) {
						String part = parts[k].strip_edges();
						if (part == "") {
							continue;
						}
						Variant v = gcl_eval_expr(part, p_locals, p_inst, cline, r_error);
						if (r_error && *r_error != "") {
							args_ok = false;
							break;
						}
						args.push_back(v);
					}
					if (args_ok && p_inst->has_function(StringName(fname))) {
						p_inst->call_function(StringName(fname), args);
						i++;
						continue;
					}
				}
			}
			gcl_eval_expr(line, p_locals, p_inst, cline, r_error);
		}
		i++;
	}
	return res;
}
