/**************************************************************************/
/*  executor.cpp                                                          */
/**************************************************************************/
/*  GCL Executor - Minimal yorumlayici (cekirdek anahtar).                */
/*  Yardimcilar:                                                          */
/*    executor_core.h  -> printf, deger/literal, typedef/enum/struct      */
/*    executor_flow.h  -> aritmetik/atama/kosul, if/while/for/do/switch   */
/*  Burada sadece satir dongusu (executor_run_ex), govde bulma ve         */
/*  fonksiyon govdelerini strip etme var.                                 */
/*  Destek:                                                               */
/*    - "void Name() { ... }" govdesini bulma                             */
/*    - "<tip> <isim> = <deger>;" atamalari                               */
/*    - PrintF/print, if/else/switch/for/while/do                         */
/*    - typedef/enum/struct/tuple/dict + obj.field / arr[i]               */
/**************************************************************************/

#include "executor.h"
#include "executor_flow.h"
#include "executor_class.h"

namespace gcl {

namespace {
using namespace executor_core;
using namespace executor_flow;
using namespace executor_class;
} // namespace

void executor_run_ex(const String &p_body, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types, int &r_exit, Variant *p_retval) {
	const int len = p_body.length();
	int line_start = 0;
	bool in_block_comment = false;
	bool region_active = true;

	while (line_start <= len && r_exit == 0) {
		int line_end = p_body.find("\n", line_start);
		if (line_end == -1) {
			line_end = len;
		}
		String raw = p_body.substr(line_start, line_end - line_start);

		/* satirin basindaki beyazlari kontrol et : "#..." preprocessor */
		int first_nonspace = 0;
		while (first_nonspace < raw.length() && (raw[first_nonspace] == ' ' || raw[first_nonspace] == '\t')) {
			first_nonspace++;
		}
		bool is_hash_line = first_nonspace < raw.length() && raw[first_nonspace] == '#';
		if (is_hash_line) {
			/* #-satirlarini strip ETME: strip_line_comment_state '#'i satir
			   yorumu sanip temizler ve "#define X" bos stringe donusur.
			   Dogrudan handle_preprocess'e ver. */
			String hash_line = raw.substr(first_nonspace).strip_edges();
			if (handle_preprocess(hash_line, p_members, p_types, region_active)) {
				line_start = line_end + 1;
				if (line_end == len) {
					break;
				}
				continue;
			}
		}

		/* pasif bolge: (#ifdef false / #elif false / #else pasif) kodu atla */
		if (!region_active) {
			line_start = line_end + 1;
			if (line_end == len) {
				break;
			}
			continue;
		}

		/* blok yorum durumunu tasiyarak satiri temizle */
		String stmt = strip_line_comment_state(raw, in_block_comment).strip_edges();

		/* Kontrol akisi satirlari (if/for/while/switch/do) kendi blok yonetimine
		   sahiptir; onlari asla birlestirme. Sadece deger/tip bildirimlerini
		   ("= {", typedef/enum/struct/union/class) cok satirli birleştir. */
		bool is_control = stmt.begins_with("if ") || stmt == "if" ||
				stmt.begins_with("for ") || stmt == "for" ||
				stmt.begins_with("while ") || stmt == "while" ||
				stmt.begins_with("switch ") || stmt == "switch" ||
				stmt == "do" || stmt.begins_with("do ");
		bool is_type_decl = stmt.begins_with("typedef") || stmt.begins_with("enum ") ||
				stmt == "enum" || stmt.begins_with("struct ") || stmt == "struct" ||
				stmt.begins_with("union ") || stmt == "union" || stmt.begins_with("class ");

		int brace_balance = 0;
		for (int b = 0; b < stmt.length(); b++) {
			if (stmt[b] == '{') {
				brace_balance++;
			} else if (stmt[b] == '}') {
				brace_balance--;
			}
		}
		/* Kontrol akisi satirlarini ASLA birlestirme: onlar kendi blok yonetimini
		   kullanir (find_block_end/handle_if_chain). Sadece literal "= {" ve
		   tip bildirimleri (typedef/enum/struct/union/class) cok satirli organik
		   olarak birlestirilmelidir. */
		while (brace_balance > 0 && line_end < len && !is_control && (is_type_decl || stmt.find("=") >= 0)) {
			line_start = line_end + 1;
			line_end = p_body.find("\n", line_start);
			if (line_end == -1) {
				line_end = len;
			}
			String next = strip_line_comment(p_body.substr(line_start, line_end - line_start)).strip_edges();
			stmt += " ";
			stmt += next;
			for (int b = 0; b < next.length(); b++) {
				if (next[b] == '{') {
					brace_balance++;
				} else if (next[b] == '}') {
					brace_balance--;
				}
			}
		}

		if (!stmt.empty()) {
			/* -------------------- kontrol akisi -------------------- */
			if (stmt == "break") {
				r_exit = 1;
				break;
			} else if (stmt == "continue") {
				r_exit = 2;
				break;
			} else if (stmt == "return" || stmt.begins_with("return ")) {
				r_exit = 3;
				if (p_retval && stmt.length() > 7) {
					*p_retval = solve_arith(stmt.substr(7).replace(";", "").strip_edges(), p_members);
				}
				break;
			} else if (stmt == "if" || stmt.begins_with("if ")) {
				int after = find_block_end(p_body, line_start);
				handle_if_chain(p_body, line_start, stmt, p_members, p_types, r_exit);
				line_start = line_start > after ? line_start : after;
				if (r_exit != 0) {
					break;
				}
				if (line_end == len) {
					break;
				}
				continue;
			} else if (stmt == "while" || stmt.begins_with("while ")) {
				int after = find_block_end(p_body, line_start);
				run_while(p_body, line_start, stmt, p_members, p_types, r_exit);
				line_start = line_start > after ? line_start : after;
				if (r_exit != 0) {
					break;
				}
				if (line_end == len) {
					break;
				}
				continue;
			} else if (stmt == "do") {
				int after = find_block_end(p_body, line_start);
				run_do_while(p_body, line_start, p_members, p_types, r_exit);
				line_start = line_start > after ? line_start : after;
				if (r_exit != 0) {
					break;
				}
				if (line_end == len) {
					break;
				}
				continue;
			} else if (stmt == "switch" || stmt.begins_with("switch ")) {
				int after = find_block_end(p_body, line_start);
				int sw_exit = run_switch(p_body, line_start, stmt, p_members, p_types);
				if (sw_exit != 0) {
					r_exit = sw_exit;
					break;
				}
				line_start = line_start > after ? line_start : after;
				if (r_exit != 0) {
					break;
				}
				if (line_end == len) {
					break;
				}
				continue;
			} else if (stmt == "for" || stmt.begins_with("for ")) {
				int after = find_block_end(p_body, line_start);
				run_for(p_body, line_start, stmt, p_members, p_types, r_exit);
				line_start = line_start > after ? line_start : after;
				if (r_exit != 0) {
					break;
				}
				if (line_end == len) {
					break;
				}
				continue;
			} else if (stmt == "}" || stmt.begins_with("}") ||
					stmt == "else" || stmt.begins_with("else ") ||
					stmt == "elif" || stmt.begins_with("elif ")) {
				/* kontrol akisi devam/kapanis satirlari: handle_if_chain zaten
				   zinciri isledi; ana donguye duserse bosverip atla. */
				line_start = line_end + 1;
				if (line_end == len) {
					break;
				}
				continue;
			} else {
				/* artirma / bilesik atama: "counter++;" sonundaki ; yi soy */
				String inc = stmt.replace(";", "").strip_edges();
				if (inc.ends_with("++") || inc.ends_with("--") ||
						(stmt.find("+=") != -1 || stmt.find("-=") != -1 || stmt.find("*=") != -1 || stmt.find("/=") != -1 || stmt.find("%=") != -1)) {
					apply_assignment(stmt, p_members);
				}
			}

			/* -------------------- tip bildirimleri -------------------- */
			if (stmt.begins_with("typedef")) {
				handle_typedef_stmt(stmt, p_members, p_types);
			} else if (stmt == "enum" || stmt.begins_with("enum ") || stmt.begins_with("enum\t")) {
				handle_enum_stmt(stmt, p_members, p_types);
			} else if (stmt == "struct" || stmt.begins_with("struct ") || stmt == "union" || stmt.begins_with("union ")) {
				handle_struct_stmt(stmt, p_types);
			} else if (stmt.begins_with("class ")) {
				handle_class_stmt(stmt, p_types);
				/* Birlesik class ifadesi metod govdelerinde PrintF gibi cagrilar
				   icerir; handle_call_full/resolve_class_call'a duserse global
				   pass'te 'child talks!' basar. Tum class satirlarini atla. */
				if (line_end == len) {
					break;
				}
				line_start = line_end + 1;
				continue;
			}

			/* -------------------- atama -------------------- */
			int eq = stmt.find("=");
			/* "+=" gibi bilesik atamalar yukarida apply_assignment ile islendi;
			   tekrar buraya duserse lhs "x +" olur ve obj member dalina (lhs ".")
			   takilip absolute yazim yapar. O yuzden burada atla. */
			if (eq > 0 && stmt[eq - 1] != '+' && stmt[eq - 1] != '-' &&
					stmt[eq - 1] != '*' && stmt[eq - 1] != '/' && stmt[eq - 1] != '%') {
				String lhs = stmt.substr(0, eq).strip_edges();
				String rhs = stmt.substr(eq + 1).strip_edges();
				if (rhs.ends_with(";")) {
					rhs = rhs.substr(0, rhs.length() - 1).strip_edges();
				}

				/* coklu bildirim: "int a, b, c = 5;" -> lhs "int a" olur,
				   kalanlar (b, c) deger paylasir. */
				String orig_lhs = lhs;
				bool multi_decl = lhs.find(",") != -1;
				if (multi_decl) {
					Vector<String> first_parts = split_top_level(lhs, ',');
					lhs = first_parts[0].strip_edges();
				}

				String type;
				String name;
				if (split_declaration(lhs, type, name) && is_ident_start(name[0])) {
					bool known_type = is_number_keyword(type) || p_types.types.has(StringName(type)) ||
							p_types.aliases.has(StringName(type)) || p_types.classes.has(StringName(type)) ||
							type == "tuple" || type == "dict" || type == "struct" || type == "enum" || type == "union" ||
							type == "NODE" || type == "REF" || type == "ABC" || type == "Node" || type == "NodeRef" ||
							type.begins_with("struct ") || type.begins_with("enum ") || type.begins_with("union ");

					if (known_type) {
						Variant value = Variant();
						if (rhs.length() >= 2 && rhs[0] == '(' && rhs[rhs.length() - 1] == ')') {
							/* tuple */
							value = parse_tuple_literal(rhs.substr(1, rhs.length() - 2), p_members);
						} else if (!rhs.empty() && rhs[0] == '{') {
							String inner = rhs.substr(1);
							if (!inner.empty() && inner[inner.length() - 1] == '}') {
								inner = inner.substr(0, inner.length() - 1);
							}
							if (type == "dict" || inner.find(":") != -1) {
								value = parse_dict_literal(inner, p_members);
							} else if (type != "dict" && (type == "struct" || p_types.types.has(StringName(type)) || inner.find("=") != -1)) {
								value = parse_struct_literal(inner, p_members);
							} else {
								value = inner.strip_edges();
							}
						} else if (p_types.classes.has(StringName(type))) {
							/* class instance: "FATHER" veya "CHILD(ThisFather)" */
							String rhs_s = rhs.strip_edges();
							String cname = rhs_s;
							Vector<String> cargs;
							int copen = rhs_s.find("(");
							if (copen != -1) {
								int cclose = rhs_s.rfind(")");
								if (cclose > copen) {
									cname = rhs_s.substr(0, copen).strip_edges();
									String cregion = rhs_s.substr(copen + 1, cclose - copen - 1);
									int cs = 0;
									for (int cj = 0; cj <= cregion.length(); cj++) {
										if (cj == cregion.length() || cregion[cj] == ',') {
											String a = cregion.substr(cs, cj - cs).strip_edges();
											if (!a.empty()) {
												cargs.push_back(a);
											}
											cs = cj + 1;
										}
									}
								}
							}
							value = make_class_instance(cname, cargs, p_members, p_types);
						} else {
							/* RHS fonksiyon cagrisi veya aritmetik/bitwise/ternary olabilir. */
							Variant call_ret;
							if (resolve_user_call(rhs, p_members, p_types, &call_ret)) {
								value = call_ret;
							} else if (resolve_class_call(rhs, p_members, p_types, &call_ret)) {
								value = call_ret;
							} else {
								value = solve_arith(rhs, p_members);
							}
						}
						p_members[StringName(name)] = value;

						/* coklu bildirim: "int a, b, c = 5;" -> hepsi ayni degeri alir */
						if (multi_decl) {
							Vector<String> multi = split_top_level(orig_lhs, ',');
							for (int mi = 1; mi < multi.size(); mi++) {
								String part = multi[mi].strip_edges();
								String mtype;
								String mname;
								if (split_declaration(part, mtype, mname) && is_ident_start(mname[0])) {
									p_members[StringName(mname)] = value;
								}
							}
						}
					}
				} else if (p_members.has(StringName(lhs.strip_edges()))) {
					/* duz atama: "x = ifade" (aritmetik / fonksiyon / class dahil) */
					Variant rvalue;
					if (resolve_user_call(rhs, p_members, p_types, &rvalue)) {
						p_members[StringName(lhs.strip_edges())] = rvalue;
					} else if (resolve_class_call(rhs, p_members, p_types, &rvalue)) {
						p_members[StringName(lhs.strip_edges())] = rvalue;
					} else {
						p_members[StringName(lhs.strip_edges())] = solve_arith(rhs, p_members);
					}
				} else if (lhs.find(".") != -1) {
					/* Obje member atamasi: "Body.Rotation.y = 5" veya
					   "NODE REF = ..." sonrasi "REF.Position.x = ..." */
					Variant rvalue = solve_arith(rhs, p_members);
					set_member_value(lhs, rvalue, p_members);
				}
			}

			/* --- cikti / class / fonksiyon cagrilari --- */
			/* "Time.Sleep(0.5)" gibi statik Time tablosu once dene. */
			if (handle_time_call(stmt, p_members, p_types)) {
				/* Sleep LOCAL'tir: kalan satirlar + scope yalnizca ILK
				   tetiklemede bir kez saklanir. Daha sonra ayni Sleep
				   satirina yeniden gelinirse (her frame Update ayni
				   fonksiyonu cagirir) sure kaydirilmaz VE bu fonksiyonun
				   kalan satirlari YINE calistirilmaz; sure dolunca
				   sleep_rest bir kez calistirilir. Ust fonksiyon (Update)
				   bu fonksiyondan sonra normal sekilde devam eder. */
				if (!p_types.sleep_rest_captured) {
					p_types.sleep_rest_captured = true;
					p_types.sleep_rest = p_body.substr(line_end + 1);
					p_types.sleep_scope.clear();
					for (const Map<StringName, Variant>::Element *E = p_members.front(); E; E = E->next()) {
						p_types.sleep_scope[E->key()] = E->get();
					}
				}
				line_start = len + 1;
				break;
			} else if (!handle_extern_call(stmt, p_members)) {
				/* self.<zincir>.<Metot>(args): "self.Raycast.Skip(self)" gibi
				   extern metod cagrilarini once dene. resolve_user_call yalnizca
				   duz "isim(args)" kalibini cozer; noktali zincirler daha once
				   sessizce yutuluyordu - bu yuzden karakter kendi capsule'ine
				   takilip "Body Name: FPSController" spam'i basiyordu. */
				if (!handle_call_full(stmt, p_members, p_types)) {
					if (!resolve_class_call(stmt, p_members, p_types, nullptr)) {
						resolve_user_call(stmt, p_members, p_types, nullptr);
					}
				}
			}
		}

		if (line_end == len) {
			break;
		}
		line_start = line_end + 1;
	}
}

/* Dış API: exit kodu yok sayilir. */
void executor_run(const String &p_body, Map<StringName, Variant> &p_members, GCLTypeRegistry &p_types) {
	int exit_code = 0;
	executor_run_ex(p_body, p_members, p_types, exit_code);
}

String executor_strip_bodies(const String &p_code) {
	const int len = p_code.length();
	String out;
	int i = 0;

	while (i < len) {
		if (p_code[i] == '{') {
			/* "= {" dizi/struct/dict baslatmasidir ve
			   "enum/struct/union/typedef ... {" tip bildirimleridir;
			   "class ... {" bildirimi de govde degildir; korunur. */
			int k = i - 1;
			while (k >= 0 && (p_code[k] == ' ' || p_code[k] == '\t')) {
				k--;
			}
			bool is_array_literal = (k >= 0 && p_code[k] == '=');

			if (!is_array_literal) {
				int line_start = k;
				while (line_start > 0 && p_code[line_start - 1] != '\n') {
					line_start--;
				}
				String prefix = p_code.substr(line_start, k - line_start + 1).strip_edges();
				if (prefix.begins_with("enum") || prefix.begins_with("struct") ||
						prefix.begins_with("union") || prefix.begins_with("typedef") ||
						prefix.begins_with("class")) {
					is_array_literal = true;
				}
			}

			int depth = 1;
			int block_start = i + 1;
			i++;
			while (i < len && depth > 0) {
				if (p_code[i] == '{') {
					depth++;
				} else if (p_code[i] == '}') {
					depth--;
				}
				i++;
			}

			if (is_array_literal) {
				out += "{";
				out += p_code.substr(block_start, i - block_start - 1);
				out += "}";
			} else {
				out += " ";
			}
		} else {
			out += p_code[i];
			i++;
		}
	}

	return out;
}

/* Fonksiyon adini p_code icinde arar; donus tipi "void" OLMADAN da bulur.
   "void foo()", "int foo()", "gcChar Call()" gibi tum imzalar. */
bool find_func_def(const String &p_code, const String &p_func, int &r_after_paren) {
	const int len = p_code.length();
	int i = 0;

	while (i < len) {
		/* p_func kelimesini tam kelime olarak ara. */
		int w = p_code.find(p_func, i);
		if (w == -1) {
			return false;
		}
		int w_end = w + p_func.length();
		bool boundary_ok =
				(w == 0 || !is_ident_char(p_code[w - 1])) &&
				(w_end >= len || !is_ident_char(p_code[w_end]));
		if (!boundary_ok) {
			i = w + p_func.length();
			continue;
		}

		/* p_func'ten sonra "(" olmali (fonksiyon tanimi). */
		int open_paren = p_code.find("(", w_end);
		if (open_paren == -1) {
			i = w + p_func.length();
			continue;
		}

		/* "(" ile p_func arasinda sadece bosluk olmali. */
		bool only_space = true;
		for (int j = w_end; j < open_paren; j++) {
			if (p_code[j] != ' ' && p_code[j] != '\t') {
				only_space = false;
				break;
			}
		}
		if (!only_space) {
			i = w + p_func.length();
			continue;
		}

		/* govde tanimi: ")" sonrasi "{" gelmeli (cagri degil). */
		int paren_depth = 0;
		int close_paren = -1;
		for (int j = open_paren; j < len; j++) {
			if (p_code[j] == '(') {
				paren_depth++;
			} else if (p_code[j] == ')') {
				paren_depth--;
				if (paren_depth == 0) {
					close_paren = j;
					break;
				}
			}
		}
		if (close_paren == -1) {
			return false;
		}

		/* ")" sonrasi (bosluklari gec) "{" mi? */
		int k = close_paren + 1;
		while (k < len && (p_code[k] == ' ' || p_code[k] == '\t')) {
			k++;
		}
		if (k < len && p_code[k] == '{') {
			r_after_paren = k;
			return true;
		}

		i = w + p_func.length();
	}

	return false;
}

bool executor_find_body(const String &p_code, const String &p_func, String &r_body) {
	/* p_func'ten once "void"/tip gelmeli; govde "{...}" ozelinde calisir. */
	int open_brace = -1;
	if (!find_func_def(p_code, p_func, open_brace)) {
		return false;
	}
	const int len = p_code.length();
		int brace_depth = 0;
	for (int j = open_brace; j < len; j++) {
		if (p_code[j] == '{') {
			brace_depth++;
		} else if (p_code[j] == '}') {
			brace_depth--;
			if (brace_depth == 0) {
				r_body = p_code.substr(open_brace + 1, j - open_brace - 1);
				return true;
			}
		}
	}
	return false;
}

} // namespace gcl
