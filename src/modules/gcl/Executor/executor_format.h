/**************************************************************************/
/*  executor_format.h                                                    */
/**************************************************************************/
/*  GCL Executor - printf formatlama (tek amacli dosya).                  */
/*  C benzeri %d %f %s %c %u %x %o %e %g + flags/width/precision.         */
/*  Amac: her executor dosyasini <800 satir tutmak.                       */
/**************************************************************************/

#ifndef GCL_EXECUTOR_FORMAT_H
#define GCL_EXECUTOR_FORMAT_H

#include "executor.h"

namespace gcl {

namespace executor_format {

inline bool is_ident_start(CharType c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

inline bool is_ident_char(CharType c) {
	return is_ident_start(c) || (c >= '0' && c <= '9');
}

inline bool is_number_keyword(const String &p_word) {
	static const char *keywords[] = {
		"int8", "int16", "int32", "int64", "int128",
		"short", "int", "long",
		"uint8", "uint16", "uint32", "uint64", "uint128",
		"unsigned", "float16", "float32", "float64", "float128",
		"float", "double", "char", "bool", "gcChar",
		"long long", "unsigned short", "unsigned int", "unsigned long", "unsigned long long",
		"long double",
		"long long int", "unsigned long int",
	};
	for (int i = 0; i < (int)(sizeof(keywords) / sizeof(keywords[0])); i++) {
		if (p_word == keywords[i]) {
			return true;
		}
	}
	return false;
}

/* Bildirim basindaki niteleyicileri (public/private/const/static/inline/global)
   yutar; "public int x", "const char *name" gibi bildirimlerde tipi dondurur. */
inline String strip_decl_qualifiers(const String &p_lhs) {
	String s = p_lhs.strip_edges();
	for (;;) {
		bool removed = false;
		static const char *quals[] = {
			"public", "private", "const", "static", "inline", "global", "register"
		};
		for (int i = 0; i < (int)(sizeof(quals) / sizeof(quals[0])); i++) {
			String q = quals[i];
			if (s == q) {
				return String();
			}
			if (s.begins_with(q + " ")) {
				s = s.substr(q.length() + 1).strip_edges();
				removed = true;
				break;
			}
		}
		if (!removed) {
			break;
		}
	}
	return s;
}

/* "<tip> <isim>" ayirir; '*' pointer, "[]"/"[N]" dizi, "@N" etiket destegi. */
inline bool split_declaration(const String &p_lhs, String &r_type, String &r_name) {
	String lhs = strip_decl_qualifiers(p_lhs);
	if (lhs.empty()) {
		return false;
	}

	int i = lhs.length() - 1;

	while (i >= 0 && (lhs[i] == ' ' || lhs[i] == '\t')) {
		i--;
	}
	if (i >= 0 && lhs[i] == '@') {
		/* "@N" boyut etiketi: "@2" -> sadece @ varsa rakamlari ye.
		   "number0" gibi isimlerde @ YOKSA soyulmaz (isim bozulmaz). */
		i--;
		while (i >= 0 && lhs[i] >= '0' && lhs[i] <= '9') {
			i--;
		}
	}

	for (;;) {
		while (i >= 0 && (lhs[i] == ' ' || lhs[i] == '\t')) {
			i--;
		}
		if (i >= 0 && lhs[i] == ']') {
			int open = -1;
			for (int j = i; j >= 0; j--) {
				if (lhs[j] == '[') {
					open = j;
					break;
				}
			}
			if (open == -1) {
				return false;
			}
			i = open - 1;
		} else {
			break;
		}
	}

	int name_end = i + 1;
	int name_start = -1;
	while (i >= 0 && is_ident_char(lhs[i])) {
		name_start = i;
		i--;
	}
	if (name_start == -1) {
		return false;
	}
	r_name = lhs.substr(name_start, name_end - name_start);

	r_type = lhs.substr(0, name_start).strip_edges();
	while (!r_type.empty() && r_type[r_type.length() - 1] == '*') {
		r_type = r_type.substr(0, r_type.length() - 1);
	}
	r_type = r_type.strip_edges();

	return !r_type.empty() && !r_name.empty();
}

/* Gecerli identifier mi? */
inline bool is_valid_identifier(const String &p_word) {
	if (p_word.empty()) {
		return false;
	}
	if (!is_ident_start(p_word[0])) {
		return false;
	}
	for (int i = 1; i < p_word.length(); i++) {
		if (!is_ident_char(p_word[i])) {
			return false;
		}
	}
	return true;
}

/* Variant -> int64_t. */
inline int64_t variant_int(const Variant &p_v) {
	switch (p_v.get_type()) {
		case Variant::BOOL:
			return p_v.booleanize() ? 1 : 0;
		case Variant::NIL:
			return 0;
		default:
			return (int64_t)p_v;
	}
}

/* Variant -> double. */
inline double variant_real(const Variant &p_v) {
	switch (p_v.get_type()) {
		case Variant::BOOL:
			return p_v.booleanize() ? 1.0 : 0.0;
		case Variant::NIL:
			return 0.0;
		default:
			return (double)p_v;
	}
}

/* Variant -> String. */
inline String variant_str(const Variant &p_v) {
	switch (p_v.get_type()) {
		case Variant::INT:
			return String::num_int64(variant_int(p_v));
		case Variant::REAL:
			return String::num_real(variant_real(p_v));
		case Variant::BOOL:
			return p_v.booleanize() ? String("true") : String("false");
		default:
			return (String)p_v;
	}
}

inline bool is_signed(CharType p_conv) {
	return p_conv == 'd' || p_conv == 'i';
}

inline bool is_unsigned(CharType p_conv) {
	return p_conv == 'u' || p_conv == 'x' || p_conv == 'X' || p_conv == 'o';
}

/* Tamsayiyi tabana cevirir (d/i onluk, x/X hex, o sekizlik). */
inline String int_to_base(int64_t p_val, CharType p_conv) {
	uint64_t v;
	bool neg = false;
	if (is_signed(p_conv) && p_val < 0) {
		neg = true;
		v = (uint64_t)(-(p_val + 1)) + 1;
	} else {
		v = (uint64_t)p_val;
	}

	String s;
	if (v == 0) {
		s = "0";
	} else {
		while (v > 0) {
			int digit = (int)(v % 10);
			v /= 10;
			String tmp;
			tmp += CharType('0' + digit);
			s = tmp + s;
		}
	}
	if (p_conv == 'x' || p_conv == 'X' || p_conv == 'o') {
		uint64_t base = (p_conv == 'o') ? 8 : 16;
		v = (uint64_t)p_val;
		s = "";
		if (v == 0) {
			s = "0";
		} else {
			while (v > 0) {
				int digit = (int)(v % base);
				v /= base;
				CharType c;
				if (digit < 10) {
					c = CharType('0' + digit);
				} else {
					c = (p_conv == 'X') ? CharType('A' + digit - 10) : CharType('a' + digit - 10);
				}
				String tmp;
				tmp += c;
				s = tmp + s;
			}
		}
	}

	if (neg) {
		s = "-" + s;
	}
	return s;
}

/* Unsigned gosterim. */
inline String uint_to_base(int64_t p_val, CharType p_conv) {
	if (p_conv == 'u') {
		if (p_val < 0) {
			uint64_t v = (uint64_t)(-(p_val + 1)) + 1;
			return String::num_int64((int64_t)v);
		}
		return String::num_int64(p_val);
	}
	return int_to_base(p_val, p_conv);
}

/* Sayiyi genislik/hizalama ile yazar. */
inline void real_pad(String &r_out, const String &p_s, int p_width, bool p_left_align, bool p_zero_pad, bool p_plus, bool p_space, bool p_neg_zero) {
	String s = p_s;

	if (p_plus && !s.begins_with("-")) {
		s = "+" + s;
	} else if (p_space && !s.begins_with("-")) {
		s = " " + s;
	}

	int len = s.length();
	if (p_width > len) {
		int pad = p_width - len;
		if (p_left_align) {
			for (int i = 0; i < pad; i++) {
				s += " ";
			}
		} else if (p_zero_pad) {
			String sign;
			if (s.begins_with("-") || s.begins_with("+")) {
				sign = s.substr(0, 1);
				s = s.substr(1);
			} else if (s.begins_with(" ")) {
				sign = " ";
				s = s.substr(1);
			}
			for (int i = 0; i < pad; i++) {
				sign += "0";
			}
			s = sign + s;
			(void)p_neg_zero;
		} else {
			for (int i = 0; i < pad; i++) {
				s = " " + s;
			}
		}
	}

	r_out += s;
}

/* Bilimsel gosterim: 1.5e+03 gibi. */
inline String real_to_scientific(double p_absv, int p_precision, bool p_upper) {
	int prec = p_precision >= 0 ? p_precision : 6;

	int exp = 0;
	double v = p_absv;
	if (v != 0.0) {
		if (v >= 10.0) {
			while (v >= 10.0) {
				v /= 10.0;
				exp++;
			}
		} else if (v < 1.0) {
			while (v < 1.0) {
				v *= 10.0;
				exp--;
			}
		}
	}

	int64_t ipart = (int64_t)v;
	double fpart = v - (double)ipart;
	double scale = 1.0;
	for (int p = 0; p < prec; p++) {
		scale *= 10.0;
	}
	int64_t frac = (int64_t)(fpart * scale + 0.5);
	if (frac >= (int64_t)scale) {
		frac -= (int64_t)scale;
		ipart += 1;
		if (ipart >= 10) {
			ipart = 1;
			exp++;
		}
	}

	String s = String::num_int64(ipart);
	if (prec > 0) {
		String frac_s = String::num_int64(frac);
		while (frac_s.length() < prec) {
			frac_s = "0" + frac_s;
		}
		s += ".";
		s += frac_s;
	}

	s += p_upper ? "E" : "e";
	if (exp >= 0) {
		s += "+";
	} else {
		s += "-";
		exp = -exp;
	}
	if (exp < 10) {
		s += "0";
	}
	s += String::num_int64(exp);
	return s;
}

/* printf formatini uygular (flags/width/precision/uzunluk). */
inline String apply_printf(const String &p_fmt, const Vector<Variant> &p_args, int &r_consumed) {
	String out;
	int ai = 0;
	const int L = p_fmt.length();

	for (int i = 0; i < L; i++) {
		if (p_fmt[i] != '%') {
			if (p_fmt[i] == '\\' && i + 1 < L) {
				CharType n = p_fmt[i + 1];
				switch (n) {
					case 'n':
						out += "\n";
						i++;
						continue;
					case 't':
						out += "\t";
						i++;
						continue;
					case 'r':
						out += "\r";
						i++;
						continue;
					case 'a':
						out += "\a";
						i++;
						continue;
					case 'b':
						out += "\b";
						i++;
						continue;
					case 'f':
						out += "\f";
						i++;
						continue;
					case 'v':
						out += "\v";
						i++;
						continue;
					case '\\':
						out += "\\";
						i++;
						continue;
					case '"':
						out += "\"";
						i++;
						continue;
					case '\'':
						out += "'";
						i++;
						continue;
					default:
						break;
				}
			}
			out += p_fmt[i];
			continue;
		}

		if (i + 1 < L && p_fmt[i + 1] == '%') {
			out += "%";
			i++;
			continue;
		}

		bool left_align = false;
		bool zero_pad = false;
		bool plus_sign = false;
		bool space_sign = false;
		bool alternate = false;

		int g = i + 1;
		bool done_flags = false;
		while (g < L && !done_flags) {
			switch (p_fmt[g]) {
				case '-':
					left_align = true;
					g++;
					break;
				case '+':
					plus_sign = true;
					g++;
					break;
				case '0':
					zero_pad = true;
					g++;
					break;
				case ' ':
					space_sign = true;
					g++;
					break;
				case '#':
					alternate = true;
					g++;
					break;
				default:
					done_flags = true;
					break;
			}
		}

		int width = 0;
		bool has_width = false;
		while (g < L && p_fmt[g] >= '0' && p_fmt[g] <= '9') {
			width = width * 10 + (p_fmt[g] - '0');
			has_width = true;
			g++;
		}

		int precision = -1;
		if (g < L && p_fmt[g] == '.') {
			g++;
			precision = 0;
			while (g < L && p_fmt[g] >= '0' && p_fmt[g] <= '9') {
				precision = precision * 10 + (p_fmt[g] - '0');
				g++;
			}
		}

		bool is_long_double = false;
		while (g < L) {
			CharType c = p_fmt[g];
			if (c == 'L') {
				is_long_double = true;
				g++;
			} else if (c == 'l') {
				if (g + 1 < L && p_fmt[g + 1] == 'l') {
					g += 2;
				} else {
					g++;
				}
			} else if (c == 'h') {
				g++;
			} else if (c == 'j' || c == 'z' || c == 't') {
				g++;
			} else {
				break;
			}
		}

		if (g >= L) {
			break;
		}
		CharType conv = p_fmt[g];
		i = g;

		if (conv == 's') {
			Variant v = (ai < p_args.size()) ? p_args[ai] : Variant();
			ai++;
			String s = variant_str(v);
			if (precision >= 0 && precision < s.length()) {
				s = s.substr(0, precision);
			}
			real_pad(out, s, width, left_align, zero_pad, false, false, false);
		} else if (conv == 'c') {
			Variant cv = (ai < p_args.size()) ? p_args[ai] : Variant();
			ai++;
			String s;
			if (cv.get_type() == Variant::STRING) {
				String str = cv;
				s = str.empty() ? String() : str.substr(0, 1);
			} else {
				long long cval = variant_int(cv);
				s += CharType(cval);
			}
			real_pad(out, s, width, left_align, zero_pad, false, false, false);
		} else if (conv == 'd' || conv == 'i' || conv == 'u' || conv == 'x' || conv == 'X' || conv == 'o') {
			int64_t ival = (ai < p_args.size()) ? variant_int(p_args[ai]) : 0;
			ai++;
			String s = int_to_base(ival, conv);
			if (is_unsigned(conv)) {
				s = uint_to_base(ival, conv);
			}
			if (alternate) {
				if (conv == 'x') {
					s = "0x" + s;
				} else if (conv == 'X') {
					s = "0X" + s;
				} else if (conv == 'o') {
					s = "0" + s;
				}
			}
			real_pad(out, s, width, left_align, zero_pad, plus_sign, space_sign && !plus_sign && is_signed(conv), ival < 0 && zero_pad);
		} else if (conv == 'f' || conv == 'F' || conv == 'e' || conv == 'E' || conv == 'g' || conv == 'G') {
			double dval = (ai < p_args.size()) ? variant_real(p_args[ai]) : 0.0;
			ai++;

			bool neg = dval < 0.0;
			double absv = neg ? -dval : dval;

			int64_t ipart = (int64_t)absv;
			double fpart_part = absv - (double)ipart;

			int prec = precision;
			if (prec < 0) {
				prec = 6;
			}
			if (is_long_double && precision < 0) {
				prec = 17;
			}

			double scale = 1.0;
			for (int p = 0; p < prec; p++) {
				scale *= 10.0;
			}
			int64_t frac = (int64_t)(fpart_part * scale + 0.5);

			if (frac >= (int64_t)scale) {
				frac -= (int64_t)scale;
				ipart += 1;
			}

			String s = String::num_int64(ipart);
			if (prec > 0) {
				String frac_s = String::num_int64(frac);
				while (frac_s.length() < prec) {
					frac_s = "0" + frac_s;
				}
				s += ".";
				s += frac_s;
			}

			if (conv == 'e' || conv == 'E') {
				s = real_to_scientific(absv, precision, conv == 'E');
				if (neg) {
					s = "-" + s;
				}
				real_pad(out, s, width, left_align, zero_pad, plus_sign && !neg, space_sign && !plus_sign && !neg, neg && zero_pad);
				continue;
			}

			if (neg) {
				s = "-" + s;
			}
			real_pad(out, s, width, left_align, zero_pad, plus_sign && !neg, space_sign && !plus_sign && !neg, neg && zero_pad);
		} else {
			out += "%";
			out += conv;
		}
	}

	r_consumed = ai;
	return out;
}

} // namespace executor_format

} // namespace gcl

#endif // GCL_EXECUTOR_FORMAT_H
