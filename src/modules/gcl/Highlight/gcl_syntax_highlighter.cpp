/**************************************************************************/
/*  gcl_syntax_highlighter.cpp                                            */
/**************************************************************************/
/*  GCL icin ozel editor syntax highlighter.                              */
/*  Kullanici tarafindan verilen 10 renk her kategoriye yayilir:           */
/*    54016f -> kontrol kelimeleri (if/else/for/while/return...)          */
/*    700294 -> tip kelimeleri (int, float, int8, void...)                */
/*    9603c6 -> identity: degisken / fonksiyon adlari                     */
/*    ad06e4 -> = operatörü ve atamalar                                   */
/*    d863ff -> @extern, #include gibi direktifler                        */
/*    4c0080 -> # yorum                                                   */
/*    5b0099 -> sayilar / degerler (30, 3.14, true/false)                 */
/*    6c01b5 -> string icerigi                                            */
/*    8200db -> string icindeki %d %5d %.2f %Lf specifier'lar             */
/*    9700ff -> PrintF / print fonksiyon adlari                           */
/**************************************************************************/

#include "gcl_syntax_highlighter.h"

#ifdef TOOLS_ENABLED

#include "editor/plugins/script_editor_plugin.h"
#include "scene/gui/text_edit.h"

namespace gcl {

String GCLSyntaxHighlighter::get_name() const {
	return "GCL";
}

List<String> GCLSyntaxHighlighter::get_supported_languages() {
	List<String> languages;
	languages.push_back("GCL");
	return languages;
}

void GCLSyntaxHighlighter::_update_cache() {
	/* Kullanici paleti: 10 renk, her kategoriye yayilir. */
	control_color = Color::html("#54016f");
	type_color = Color::html("#700294");
	identity_color = Color::html("#9603c6");
	operator_color = Color::html("#ad06e4");
	directive_color = Color::html("#d863ff");
	comment_color = Color::html("#4c0080");
	number_color = Color::html("#5b0099");
	string_color = Color::html("#6c01b5");
	spec_color = Color::html("#8200db");
	function_color = Color::html("#9700ff");
}

static bool is_type_keyword(const String &p_word) {
	static const char *types[] = {
		"int8", "int16", "int32", "int64", "int128",
		"uint8", "uint16", "uint32", "uint64", "uint128",
		"float16", "float32", "float64", "float128",
		"short", "int", "long", "unsigned", "float", "double",
		"char", "bool", "void", "gcChar",
	};
	for (int i = 0; i < (int)(sizeof(types) / sizeof(types[0])); i++) {
		if (p_word == types[i]) {
			return true;
		}
	}
	return false;
}

static bool is_control_keyword(const String &p_word) {
	static const char *words[] = {
		"if", "elif", "else", "switch", "case", "default",
		"break", "continue", "return", "for", "while", "do",
		"const", "sizeof", "struct", "enum", "typedef", "union", "static",
	};
	for (int i = 0; i < (int)(sizeof(words) / sizeof(words[0])); i++) {
		if (p_word == words[i]) {
			return true;
		}
	}
	return false;
}

static bool is_print_function(const String &p_word) {
	/* Calisan yazimlar: PrintF ve print (kucuk harf printf gecersiz). */
	return p_word == "PrintF" || p_word == "print";
}

static bool is_ident_start(CharType c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_ident_char(CharType c) {
	return is_ident_start(c) || (c >= '0' && c <= '9');
}

static bool is_operator_char(CharType c) {
	return c == '=' || c == '+' || c == '-' || c == '*' || c == '/' ||
			c == '%' || c == '<' || c == '>' || c == '!' || c == '&' ||
			c == '|' || c == '^' || c == '~';
}

/* printf format specifier'ini dizinin uzunlugu ile birlikte tarar.
   Format: % [flag*] [width] [.precision] [length] conversion */
static int printf_spec_length(const String &p_line, int p_at) {
	const int L = p_line.length();
	int i = p_at + 1; /* '%' den sonra */

	while (i < L) {
		CharType c = p_line[i];
		if (c == '-' || c == '+' || c == ' ' || c == '0' || c == '#') {
			i++;
		} else {
			break;
		}
	}

	while (i < L && p_line[i] >= '0' && p_line[i] <= '9') {
		i++;
	}

	if (i < L && p_line[i] == '.') {
		i++;
		while (i < L && p_line[i] >= '0' && p_line[i] <= '9') {
			i++;
		}
	}

	while (i < L) {
		CharType c = p_line[i];
		if (c == 'L' || c == 'l' || c == 'h' || c == 'j' || c == 'z' || c == 't') {
			i++;
		} else {
			break;
		}
	}

	if (i < L) {
		CharType c = p_line[i];
		if (c == 'd' || c == 'i' || c == 'u' || c == 'x' || c == 'X' ||
				c == 'o' || c == 'f' || c == 'F' || c == 'e' || c == 'E' ||
				c == 'g' || c == 'G' || c == 'c' || c == 's' || c == 'p' || c == '%') {
			return i - p_at + 1;
		}
	}

	return 2; /* % sonrasi tek karakter */
}

Map<int, TextEdit::HighlighterInfo> GCLSyntaxHighlighter::_get_line_syntax_highlighting(int p_line) {
	Map<int, TextEdit::HighlighterInfo> colors;
	TextEdit *te = get_text_editor();
	if (!te) {
		return colors;
	}

	String line = te->get_line(p_line);
	const int L = line.length();
	int i = 0;
	bool in_string = false;

	while (i < L) {
		CharType c = line[i];

		if (in_string) {
			if (c == '"') {
				in_string = false;
				TextEdit::HighlighterInfo hi;
				hi.color = string_color;
				colors[i] = hi;
				i++;
				continue;
			}
			if (c == '%') {
				int slen = printf_spec_length(line, i);
				TextEdit::HighlighterInfo hi;
				hi.color = spec_color;
				for (int k = i; k < i + slen && k < L; k++) {
					colors[k] = hi;
				}
				i += slen;
				continue;
			}
			TextEdit::HighlighterInfo hi;
			hi.color = string_color;
			colors[i] = hi;
			i++;
			continue;
		}

		/* direktif: @extern, #include, #define... */
		if (c == '@' || c == '#') {
			TextEdit::HighlighterInfo hi;
			hi.color = directive_color;
			int k = i;
			while (k < L && line[k] != ' ' && line[k] != '\t') {
				colors[k] = hi;
				k++;
			}
			/* @extern + tip adi da direktif renginde */
			if (c == '@' && k < L && line[k] == ' ') {
				int start = k + 1;
				while (start < L && line[start] == ' ') {
					start++;
				}
				int end = start;
				while (end < L && is_ident_char(line[end])) {
					end++;
				}
				for (int t = start; t < end; t++) {
					colors[t] = hi;
				}
			}
			i = k;
			continue;
		}

		/* string baslangici */
		if (c == '"') {
			in_string = true;
			TextEdit::HighlighterInfo hi;
			hi.color = string_color;
			colors[i] = hi;
			i++;
			continue;
		}

		/* yorum: # (satir sonuna kadar) */
		if (c == '#') {
			TextEdit::HighlighterInfo hi;
			hi.color = comment_color;
			for (int k = i; k < L; k++) {
				colors[k] = hi;
			}
			break;
		}

		/* oparetörler */
		if (is_operator_char(c)) {
			TextEdit::HighlighterInfo hi;
			hi.color = operator_color;
			int k = i;
			while (k < L && is_operator_char(line[k])) {
				colors[k] = hi;
				k++;
			}
			i = k;
			continue;
		}

		/* sayi */
		if ((c >= '0' && c <= '9') || (c == '.' && i + 1 < L && line[i + 1] >= '0' && line[i + 1] <= '9')) {
			int start = i;
			while (i < L && ((line[i] >= '0' && line[i] <= '9') || line[i] == '.' || line[i] == 'e' || line[i] == 'E' || line[i] == '-' || line[i] == '+')) {
				i++;
			}
			TextEdit::HighlighterInfo hi;
			hi.color = number_color;
			for (int k = start; k < i; k++) {
				colors[k] = hi;
			}
			continue;
		}

		/* kelime (identifier) */
		if (is_ident_start(c)) {
			int start = i;
			while (i < L && is_ident_char(line[i])) {
				i++;
			}
			String word = line.substr(start, i - start);

			if (is_print_function(word)) {
				TextEdit::HighlighterInfo hi;
				hi.color = function_color;
				for (int k = start; k < i; k++) {
					colors[k] = hi;
				}
			} else if (is_type_keyword(word)) {
				TextEdit::HighlighterInfo hi;
				hi.color = type_color;
				for (int k = start; k < i; k++) {
					colors[k] = hi;
				}
			} else if (is_control_keyword(word)) {
				TextEdit::HighlighterInfo hi;
				hi.color = control_color;
				for (int k = start; k < i; k++) {
					colors[k] = hi;
				}
			} else if (word == "true" || word == "false" || word == "null") {
				TextEdit::HighlighterInfo hi;
				hi.color = number_color;
				for (int k = start; k < i; k++) {
					colors[k] = hi;
				}
			} else {
				/* identity: degisken / fonksiyon adlari */
				TextEdit::HighlighterInfo hi;
				hi.color = identity_color;
				for (int k = start; k < i; k++) {
					colors[k] = hi;
				}
			}
			continue;
		}

		i++;
	}

	return colors;
}

static SyntaxHighlighter *create_gcl_syntax_highlighter() {
	return memnew(GCLSyntaxHighlighter);
}

void register_gcl_syntax_highlighter() {
	ScriptEditor::register_create_syntax_highlighter_function(create_gcl_syntax_highlighter);
}

} // namespace gcl

#endif // TOOLS_ENABLED
