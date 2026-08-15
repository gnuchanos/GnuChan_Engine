/**************************************************************************/
/*  executor_ops.h                                                       */
/**************************************************************************/
/*  GCL Executor - Aritmetik/bitwise/ternary/sizeof islemleri.            */
/*  C benzeri operatorler + GCL typedef operator isimleri (and/or/not).   */
/*  Amac: her executor dosyasini <800 satir tutmak.                       */
/**************************************************************************/

#ifndef GCL_EXECUTOR_OPS_H
#define GCL_EXECUTOR_OPS_H

#include "executor_format.h"

namespace gcl {

namespace executor_ops {

using executor_format::is_ident_char;
using executor_format::variant_real;
using executor_format::variant_str;

/* executor_core::initialize_value'nun ileri bildirimi (executor_core.h
   bu dosyayi include eder ve gercegini tanimlar). */
inline Variant initialize_value_fwd(const String &p_rhs, const Map<StringName, Variant> &p_members);

/* Variant'i mantiksal deger olarak degerlendirir (preprocessor #if icin). */
inline bool normalize_solve_bool(const Variant &p_v) {
	if (p_v.get_type() == Variant::BOOL) {
		return p_v.booleanize();
	}
	if (p_v.get_type() == Variant::NIL) {
		return false;
	}
	if (p_v.get_type() == Variant::STRING) {
		String s = p_v;
		return !s.empty() && s != "false" && s != "0";
	}
	return variant_real(p_v) != 0.0;
}

/* operator typedef isimlerini C karsiliklarina cevirir. */
inline String normalize_operator_word(const String &p_word) {
	if (p_word == "and") {
		return "&&";
	}
	if (p_word == "or") {
		return "||";
	}
	if (p_word == "not") {
		return "!";
	}
	if (p_word == "notEquals") {
		return "!=";
	}
	if (p_word == "equals") {
		return "==";
	}
	if (p_word == "bitAnd") {
		return "&";
	}
	if (p_word == "bitOr") {
		return "|";
	}
	if (p_word == "bitXor") {
		return "^";
	}
	if (p_word == "bitNot") {
		return "~";
	}
	if (p_word == "leftShift") {
		return "<<";
	}
	if (p_word == "rightShift") {
		return ">>";
	}
	return p_word;
}

/* Ifadede operator typedef sozcuklerini C'ye cevirir. */
inline String replace_operator_words(const String &p_expr) {
	String s = p_expr;
	s = s.replace("notEquals", "!=");
	s = s.replace("equals", "==");
	s = s.replace("leftShift", "<<");
	s = s.replace("rightShift", ">>");
	s = s.replace("bitAnd", "&");
	s = s.replace("bitOr", "|");
	s = s.replace("bitXor", "^");
	s = s.replace("bitNot", "~");

	String out;
	int i = 0;
	const int L = s.length();
	while (i < L) {
		if (is_ident_char(s[i])) {
			int j = i;
			while (j < L && is_ident_char(s[j])) {
				j++;
			}
			String w = s.substr(i, j - i);
			out += normalize_operator_word(w);
			i = j;
		} else {
			out += s[i];
			i++;
		}
	}
	return out;
}

/* sizeof(type) / sizeof(variable) -> bayt. */
inline int sizeof_type(const String &p_name) {
	String t = p_name.strip_edges();
	if (t == "char" || t == "bool" || t == "int8" || t == "uint8" || t == "gcChar") {
		return 1;
	}
	if (t == "short" || t == "int16" || t == "uint16" || t == "float16") {
		return 2;
	}
	if (t == "int" || t == "float" || t == "int32" || t == "uint32" || t == "float32") {
		return 4;
	}
	if (t == "long" || t == "double" || t == "int64" || t == "uint64" || t == "float64" || t == "long long") {
		return 8;
	}
	if (t == "int128" || t == "uint128" || t == "float128" || t == "long double") {
		return 16;
	}
	if (t.ends_with("*") || t.ends_with("[]")) {
		return 8;
	}
	return 4;
}

/* initialize_value'dan once sizeof isler.
   TANIM executor_core.h'in sonundadir (initialize_value_fwd'nin inline
   tanimi gorunur olduktan sonra) — boylece MSVC dis baglanti referansi
   (LNK2019) uretmez. */
Variant ops_initialize_value(const String &p_rhs, const Map<StringName, Variant> &p_members);

/* Bir ifadede en son islenecek operatoru bulur (oncelik sirasi). */
inline bool find_best_op(const String &p_s, int &r_pos, CharType &r_op) {
	const int L = p_s.length();
	int depth = 0;

	for (int i = 0; i < L; i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && i > 0) {
			if (c == '|' && i + 1 < L && p_s[i + 1] == '|') {
				r_pos = i;
				r_op = '|';
				return true;
			}
			if (c == '&' && i + 1 < L && p_s[i + 1] == '&') {
				r_pos = i;
				r_op = '&';
				return true;
			}
		}
	}

	static const CharType bitwise_ops[] = { '|', '^', '&' };
	for (int oi = 0; oi < 3; oi++) {
		depth = 0;
		for (int i = 0; i < L; i++) {
			CharType c = p_s[i];
			if (c == '(') {
				depth++;
			} else if (c == ')') {
				depth--;
			} else if (depth == 0 && i > 0 && c == bitwise_ops[oi]) {
				if (c == '|' && i + 1 < L && p_s[i + 1] == '|') {
					continue;
				}
				if (c == '&' && i + 1 < L && p_s[i + 1] == '&') {
					continue;
				}
				r_pos = i;
				r_op = c;
				return true;
			}
		}
	}

	depth = 0;
	for (int i = 0; i < L; i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && i > 0) {
			if (c == '=' && i + 1 < L && p_s[i + 1] == '=') {
				r_pos = i;
				r_op = '=';
				return true;
			}
			if (c == '!' && i + 1 < L && p_s[i + 1] == '!') {
				r_pos = i;
				r_op = '!';
				return true;
			}
		}
	}

	depth = 0;
	for (int i = 0; i < L; i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && i > 0 && (c == '<' || c == '>')) {
			if ((c == '<' && i + 1 < L && p_s[i + 1] == '<') ||
					(c == '>' && i + 1 < L && p_s[i + 1] == '>')) {
				/* Shift operatoru (<< / >>): kendi dongusunde ele alinir.
				   IKINCI karakteri de atla - yoksa "1 << 4" ifadesinde
				   ikinci '<' karistirma operatoru sanilir ve 16 yerine
				   "1 < (4)" calisip sonuc 1 doner. */
				i++;
				continue;
			}
			r_pos = i;
			r_op = c;
			return true;
		}
	}

	depth = 0;
	for (int i = 0; i < L; i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && i > 0) {
			if (c == '<' && i + 1 < L && p_s[i + 1] == '<') {
				r_pos = i;
				r_op = '<';
				return true;
			}
			if (c == '>' && i + 1 < L && p_s[i + 1] == '>') {
				r_pos = i;
				r_op = '>';
				return true;
			}
		}
	}

	depth = 0;
	for (int i = 0; i < L; i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && i > 0 && (c == '+' || c == '-')) {
			r_pos = i;
			r_op = c;
			return true;
		}
	}

	depth = 0;
	for (int i = 0; i < L; i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && i > 0 && (c == '*' || c == '/' || c == '%')) {
			r_pos = i;
			r_op = c;
			return true;
		}
	}

	return false;
}

/* solve_arith ileri bildirimi (try_ternary ondan once tanimlanir; kosul/
   secilen dal karsilastirma icerebilir). */
inline Variant solve_arith(const String &p_rhs, const Map<StringName, Variant> &p_members);

/* Ternary: "kosul ? dogru : yanlis". */
inline bool try_ternary(const String &p_s, const Map<StringName, Variant> &p_members, Variant &r_out) {
	int depth = 0;
	int q_pos = -1;
	for (int i = 0; i < p_s.length(); i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && c == '?') {
			q_pos = i;
			break;
		}
	}
	if (q_pos == -1) {
		return false;
	}
	int col_pos = -1;
	depth = 0;
	for (int i = q_pos + 1; i < p_s.length(); i++) {
		CharType c = p_s[i];
		if (c == '(') {
			depth++;
		} else if (c == ')') {
			depth--;
		} else if (depth == 0 && c == ':') {
			col_pos = i;
			break;
		}
	}
	if (col_pos == -1) {
		return false;
	}

	String cond = p_s.substr(0, q_pos).strip_edges();
	String t = p_s.substr(q_pos + 1, col_pos - q_pos - 1).strip_edges();
	String f = p_s.substr(col_pos + 1).strip_edges();

	String cn = replace_operator_words(cond);
	if (cn.begins_with("(") && cn.ends_with(")")) {
		cn = cn.substr(1, cn.length() - 2);
	}
	/* Kosul "EDSF > 20" gibi karsilastirma olabilir; initialize_value bunu
	   cozemez. solve_arith ile coz (ternary icermedigi surece geri donus yok). */
	Variant cv = solve_arith(cn, p_members);
	bool cval = normalize_solve_bool(cv);

	String chosen = cval ? t : f;
	r_out = solve_arith(chosen, p_members);
	return true;
}

/* Aritmetik ifadeyi cozer: + - * / % & | ^ ~ << >> == != < > <= >=
   && || ve ternary. GCL typedef operator adlarini da tanir. */
inline Variant solve_arith(const String &p_rhs, const Map<StringName, Variant> &p_members) {
	String r = replace_operator_words(p_rhs).strip_edges();
	if (r.empty()) {
		return Variant();
	}

	Variant tv;
	if (try_ternary(r, p_members, tv)) {
		return tv;
	}

	if (r.begins_with("(") && r.ends_with(")")) {
		int depth = 0;
		bool all_wrapped = true;
		for (int i = 0; i < r.length(); i++) {
			if (r[i] == '(') {
				depth++;
			} else if (r[i] == ')') {
				depth--;
				if (depth == 0 && i < r.length() - 1) {
					all_wrapped = false;
					break;
				}
			}
		}
		if (all_wrapped) {
			return solve_arith(r.substr(1, r.length() - 2), p_members);
		}
	}

	if (r[0] == '!' || r[0] == '~' || r[0] == '-') {
		CharType uop = r[0];
		Variant inner = solve_arith(r.substr(1).strip_edges(), p_members);
		if (uop == '!') {
			return Variant(!(variant_real(inner) != 0.0));
		}
		if (uop == '~') {
			return Variant((double)(~(int64_t)variant_real(inner)));
		}
		if (uop == '-') {
			return Variant(-variant_real(inner));
		}
	}

	int op_pos = -1;
	CharType op = 0;
	if (find_best_op(r, op_pos, op)) {
		String lhs_s = r.substr(0, op_pos).strip_edges();
		bool is_shift = (op == '<' && op_pos + 1 < r.length() && r[op_pos + 1] == '<') ||
				(op == '>' && op_pos + 1 < r.length() && r[op_pos + 1] == '>');
		bool is_inclusive = (op == '<' || op == '>') && op_pos + 1 < r.length() && r[op_pos + 1] == '=';
		/* "<=" / ">=" : RHS'teki '=' de tüketilir (yoksa "= n" kalir). */
		String rhs_s = (is_shift || is_inclusive) ? r.substr(op_pos + 2).strip_edges() : r.substr(op_pos + 1).strip_edges();

		Variant lhs = solve_arith(lhs_s, p_members);
		Variant rhs = solve_arith(rhs_s, p_members);

		if (op == '|' && op_pos + 1 < r.length() && r[op_pos + 1] == '|') {
			return Variant((variant_real(lhs) != 0.0) || (variant_real(rhs) != 0.0));
		}
		if (op == '&' && op_pos + 1 < r.length() && r[op_pos + 1] == '&') {
			return Variant((variant_real(lhs) != 0.0) && (variant_real(rhs) != 0.0));
		}
		if (op == '=') {
			return Variant(variant_str(lhs) == variant_str(rhs));
		}
		if (op == '!') {
			return Variant(variant_str(lhs) != variant_str(rhs));
		}
		if (op == '<' && !is_shift) {
			return Variant(is_inclusive ? (variant_real(lhs) <= variant_real(rhs)) : (variant_real(lhs) < variant_real(rhs)));
		}
		if (op == '>' && !is_shift) {
			return Variant(is_inclusive ? (variant_real(lhs) >= variant_real(rhs)) : (variant_real(lhs) > variant_real(rhs)));
		}
		if (op == '<') {
			return Variant((double)((int64_t)variant_real(lhs) << (int64_t)variant_real(rhs)));
		}
		if (op == '>') {
			return Variant((double)((int64_t)variant_real(lhs) >> (int64_t)variant_real(rhs)));
		}

		double lv = variant_real(lhs);
		double rv = variant_real(rhs);
		switch (op) {
			case '+':
				return lv + rv;
			case '-':
				return lv - rv;
			case '*':
				return lv * rv;
			case '/':
				return rv == 0.0 ? Variant() : Variant(lv / rv);
			case '%':
				return rv == 0.0 ? Variant() : Variant((double)((int64_t)lv % (int64_t)rv));
			case '&':
				return Variant((double)((int64_t)lv & (int64_t)rv));
			case '|':
				return Variant((double)((int64_t)lv | (int64_t)rv));
			case '^':
				return Variant((double)((int64_t)lv ^ (int64_t)rv));
		}
		return Variant();
	}

	return ops_initialize_value(r, p_members);
}

} // namespace executor_ops

} // namespace gcl

#endif // GCL_EXECUTOR_OPS_H
