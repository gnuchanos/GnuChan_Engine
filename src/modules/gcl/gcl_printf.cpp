/**************************************************************************/
/*  gcl_printf.cpp                                                        */
/**************************************************************************/
/*  printf formatting & console output. Small standalone module.          */
/*                                                                        */
/*  Conversions (gcl_language.md "PRINTF"):                               */
/*    %d %i %u %x %X %o  signed/unsigned integers, hex, octal            */
/*    %f %e %g           floating point                                  */
/*    %s %c %p           string, char, pointer                           */
/*    %n                 GCL extension: node name / node list            */
/*    %%                 literal percent                                 */
/*  Length modifiers: hh h l ll L z (accepted; values are stored as      */
/*    32-bit ints in this engine's Variant, so they only affect parsing) */
/*  Width & precision: %5d %-5d %05d %.2f %8.2f                          */
/*  Escapes: \n \t \r \\ \" \'                                            */
/**************************************************************************/

#include "core/array.h"
#include "core/object.h"
#include "core/print_string.h"
#include "gcl_core.h"

namespace {

int64_t gcl_printf_to_int(const Variant &p_v) {
	if (p_v.get_type() == Variant::REAL) {
		return (int64_t)(double)p_v;
	}
	return (int64_t)(int)p_v;
}

double gcl_printf_to_double(const Variant &p_v) {
	if (p_v.get_type() == Variant::REAL) {
		return (double)p_v;
	}
	return (double)(int)p_v;
}

String gcl_printf_spaces(int p_n) {
	String r;
	for (int k = 0; k < p_n; k++) {
		r += " ";
	}
	return r;
}

String gcl_printf_zeros(int p_n) {
	String r;
	for (int k = 0; k < p_n; k++) {
		r += "0";
	}
	return r;
}

/* Applies minimum width, zero padding and left alignment. */
String gcl_printf_width(const String &p_s, int p_width, bool p_zero, bool p_left) {
	if (p_width <= 0 || p_s.length() >= p_width) {
		return p_s;
	}
	int gap = p_width - p_s.length();
	if (p_zero && !p_left) {
		String out;
		int idx = 0;
		if (p_s.length() > 0 && (p_s[0] == '-' || p_s[0] == '+')) {
			out += String::chr(p_s[0]);
			idx = 1;
		}
		out += gcl_printf_zeros(gap);
		out += p_s.substr(idx);
		return out;
	}
	if (p_left) {
		return p_s + gcl_printf_spaces(gap);
	}
	return gcl_printf_spaces(gap) + p_s;
}

/* Integer conversions: d i u x X o, with precision zero-padding. */
String gcl_printf_int(int64_t p_val, const String &p_conv, int p_prec, int p_width, bool p_zero, bool p_left) {
	String body;
	if (p_conv == "x") {
		body = String::num_int64(p_val, 16);
	} else if (p_conv == "X") {
		body = String::num_int64(p_val, 16).to_upper();
	} else if (p_conv == "o") {
		body = String::num_int64(p_val, 8);
	} else {
		body = itos(p_val);
	}
	if (p_prec > 0 && body.length() < p_prec) {
		String sign;
		if (body.begins_with("-")) {
			sign = "-";
			body = body.substr(1);
		}
		body = sign + gcl_printf_zeros(p_prec - body.length()) + body;
	}
	return gcl_printf_width(body, p_width, p_zero, p_left);
}

/* Float conversions: f e g. C default precision is 6 decimals for %f/%e
 * and shortest form for %g (trailing zeros kept). */
String gcl_printf_float(double p_val, const String &p_conv, int p_prec, int p_width, bool p_zero, bool p_left) {
	bool is_exp = (p_conv == "e" || p_conv == "E");
	bool is_gen = (p_conv == "g" || p_conv == "G");
	int eff = (p_prec < 0) ? 6 : p_prec;
	String body;
	if (is_exp) {
		body = String::num_scientific(p_val);
	} else if (is_gen) {
		body = String::num(p_val);
	} else {
		body = String::num(p_val, eff);
	}
	return gcl_printf_width(body, p_width, p_zero, p_left);
}

} // namespace

String gcl_printf_format(const String &p_fmt, const Vector<Variant> &p_args) {
	String out;
	int arg_idx = 0;
	int i = 0;
	const int L = p_fmt.length();
	while (i < L) {
		CharType c = p_fmt[i];
		if (c == '%' && i + 1 < L) {
			int j = i + 1;
			if (p_fmt[j] == '%') {
				out += "%";
				i = j + 1;
				continue;
			}
			bool left = false;
			bool zero = false;
			while (j < L) {
				CharType f = p_fmt[j];
				if (f == '-') {
					left = true;
				} else if (f == '0') {
					zero = true;
				} else {
					break;
				}
				j++;
			}
			int width = 0;
			while (j < L && p_fmt[j] >= '0' && p_fmt[j] <= '9') {
				width = width * 10 + (p_fmt[j] - '0');
				j++;
			}
			int prec = -1;
			if (j < L && p_fmt[j] == '.') {
				j++;
				prec = 0;
				while (j < L && p_fmt[j] >= '0' && p_fmt[j] <= '9') {
					prec = prec * 10 + (p_fmt[j] - '0');
					j++;
				}
			}
			/* Length modifiers: hh h l ll L z. */
			if (j + 1 < L && p_fmt[j] == 'l' && p_fmt[j + 1] == 'l') {
				j += 2;
			} else if (j + 1 < L && p_fmt[j] == 'h' && p_fmt[j + 1] == 'h') {
				j += 2;
			} else if (j < L && (p_fmt[j] == 'l' || p_fmt[j] == 'h' || p_fmt[j] == 'L' || p_fmt[j] == 'z')) {
				j++;
			}
			if (j < L) {
				CharType conv = p_fmt[j];
				String conv_str = String::chr(conv);
				if (conv == 'n') {
					/* GCL extension: node name / node list */
					if (arg_idx < p_args.size()) {
						Variant a = p_args[arg_idx];
						Object *o = a;
						if (o && o->is_class("Node")) {
							out += (String)o->call("get_name");
						} else if (a.get_type() == Variant::ARRAY) {
							Array arr = a;
							for (int k = 0; k < arr.size(); k++) {
								if (k > 0) {
									out += ", ";
								}
								Object *el = arr[k];
								if (el && el->is_class("Node")) {
									out += (String)el->call("get_name");
								} else {
									out += (String)arr[k];
								}
							}
						} else {
							out += (String)a;
						}
					}
					arg_idx++;
					i = j + 1;
					continue;
				}
				if (conv == 'd' || conv == 'i' || conv == 'u' || conv == 'x' || conv == 'X' || conv == 'o') {
					if (arg_idx < p_args.size()) {
						out += gcl_printf_int(gcl_printf_to_int(p_args[arg_idx]), conv_str, prec, width, zero, left);
					}
					arg_idx++;
					i = j + 1;
					continue;
				}
				if (conv == 'f' || conv == 'F' || conv == 'e' || conv == 'E' || conv == 'g' || conv == 'G') {
					if (arg_idx < p_args.size()) {
						out += gcl_printf_float(gcl_printf_to_double(p_args[arg_idx]), conv_str, prec, width, zero, left);
					}
					arg_idx++;
					i = j + 1;
					continue;
				}
				if (conv == 's') {
					String s;
					if (arg_idx < p_args.size()) {
						s = (String)p_args[arg_idx];
					}
					out += gcl_printf_width(s, width, false, left);
					arg_idx++;
					i = j + 1;
					continue;
				}
				if (conv == 'c') {
					if (arg_idx < p_args.size()) {
						out += gcl_printf_width(String::chr((CharType)(int)p_args[arg_idx]), width, false, left);
					}
					arg_idx++;
					i = j + 1;
					continue;
				}
				if (conv == 'p') {
					if (arg_idx < p_args.size()) {
						out += "0x" + String::num_int64(gcl_printf_to_int(p_args[arg_idx]), 16);
					}
					arg_idx++;
					i = j + 1;
					continue;
				}
			}
		}
		if (c == '\\' && i + 1 < L) {
			CharType n = p_fmt[i + 1];
			if (n == 'n') {
				out += "\n";
				i += 2;
				continue;
			}
			if (n == 't') {
				out += "\t";
				i += 2;
				continue;
			}
			if (n == 'r') {
				out += "\r";
				i += 2;
				continue;
			}
			if (n == '\\') {
				out += "\\";
				i += 2;
				continue;
			}
			if (n == '"') {
				out += "\"";
				i += 2;
				continue;
			}
			if (n == '\'') {
				out += "'";
				i += 2;
				continue;
			}
		}
		out += String::chr(c);
		i++;
	}
	return out;
}

void gcl_printf_output(const String &p_text) {
	Vector<String> lines = p_text.split("\n");
	if (lines.size() > 0 && lines[lines.size() - 1] == "") {
		lines.resize(lines.size() - 1);
	}
	for (int i = 0; i < lines.size(); i++) {
		print_line(lines[i]);
	}
}
