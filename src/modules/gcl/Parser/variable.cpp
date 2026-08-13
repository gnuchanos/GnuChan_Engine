/**************************************************************************/
/*  variable.cpp                                                          */
/**************************************************************************/
/*  GCL Parser - Variable modulu.                                         */
/*  "<sayi tipi> <isim>" bildirimlerini bulur;                              */
/*  otomatik tamamlama normal degisken listesini besler.                  */
/**************************************************************************/

#include "parser.h"

namespace gcl {

/* GCL sayi tipi kelimeleri (number.cpp ile uyumlu). */
static bool is_number_keyword(const String &p_word) {
	static const char *keywords[] = {
		"int8", "int16", "int32", "int64", "int128",
		"short", "int", "long",
		"uint8", "uint16", "uint32", "uint64", "uint128",
		"unsigned", "float16", "float32", "float64", "float128",
		"float", "double", "gcChar", "tuple", "dict",
	};
	for (int i = 0; i < (int)(sizeof(keywords) / sizeof(keywords[0])); i++) {
		if (p_word == keywords[i]) {
			return true;
		}
	}
	return false;
}

/* Bir isim gecerli mi? harf/_ ile baslar, harf/rakam/_ devam eder. */
static bool is_valid_identifier(const String &p_word) {
	const int len = p_word.length();
	if (len == 0) {
		return false;
	}
	CharType c = p_word[0];
	if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')) {
		return false;
	}
	for (int i = 1; i < len; i++) {
		c = p_word[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
			return false;
		}
	}
	return true;
}

/* Bir kelimeden isim soyli eklerini temizler:
   bastaki '*' (pointer), sondaki "[]" / "[N]" / "@N" (dizi/boyut).
   "***games" -> "games", "TEXT[]" -> "TEXT", "games[][32] @3" -> "games". */
static String clean_identifier(const String &p_word) {
	String w = p_word;

	/* sondaki @N etiketi */
	if (w.ends_with("@") || w.find("@") != -1) {
		int at = w.find("@");
		w = w.substr(0, at);
	}

	/* sondaki [...] gruplari */
	for (;;) {
		if (w.ends_with("]")) {
			int open = w.rfind("[");
			if (open == -1) {
				break;
			}
			w = w.substr(0, open);
		} else {
			break;
		}
	}

	/* bastaki pointerlar */
	while (w.begins_with("*")) {
		w = w.substr(1);
	}

	return w;
}

void variable_scan(const String &p_code, Vector<String> &r_names) {
	r_names.clear();

	/* Satir satir: kelimeleri ayir, tip gorunce sonraki kelime isim. */
	int line_start = 0;
	const int len = p_code.length();

	while (line_start <= len) {
		int line_end = p_code.find("\n", line_start);
		if (line_end == -1) {
			line_end = len;
		}

		String line = p_code.substr(line_start, line_end - line_start);
		line = line.strip_edges();

		/* @extern / # ile baslayan satirlari atla */
		if (line.begins_with("@extern") || line.begins_with("#")) {
			if (line_end == len) {
				break;
			}
			line_start = line_end + 1;
			continue;
		}

		/* Kelime kelime ayir */
		Vector<String> words;
		int i = 0;
		const int llen = line.length();
		while (i < llen && (line[i] == ' ' || line[i] == '\t')) {
			i++;
		}
		while (i < llen) {
			int wstart = i;
			while (i < llen && line[i] != ' ' && line[i] != '\t' && line[i] != ';' && line[i] != '=' && line[i] != '(' && line[i] != ')') {
				i++;
			}
			if (i > wstart) {
				words.push_back(line.substr(wstart, i - wstart));
			}
			while (i < llen && (line[i] == ' ' || line[i] == '\t' || line[i] == ';' || line[i] == '=' || line[i] == '(' || line[i] == ')')) {
				i++;
			}
		}

		/* Cok kelimeli tipleri yakala: 3 kelime once, sonra 2 kelime.
		   "long long int X" / "unsigned long int X" / "unsigned long long X"
		   ve "unsigned short|int|long X", "long long X", "long double X". */
		const int wc = words.size();
		for (int k = 0; k < wc; k++) {
			bool is_type = false;

			/* 3 kelimeli: "unsigned long int X" */
			if (words[k] == "unsigned" && k + 3 < wc && words[k + 1] == "long" &&
					(k + 2 < wc) && words[k + 2] == "int") {
				if (is_valid_identifier(words[k + 3])) {
					r_names.push_back(words[k + 3]);
				}
				is_type = true;
			}
			/* 3 kelimeli: "unsigned long long X" */
			else if (words[k] == "unsigned" && k + 3 < wc && words[k + 1] == "long" && words[k + 2] == "long") {
				if (is_valid_identifier(words[k + 3])) {
					r_names.push_back(words[k + 3]);
				}
				is_type = true;
			}
			/* 3 kelimeli: "long long int X" */
			else if (words[k] == "long" && k + 3 < wc && words[k + 1] == "long" && words[k + 2] == "int") {
				if (is_valid_identifier(words[k + 3])) {
					r_names.push_back(words[k + 3]);
				}
				is_type = true;
			}
			/* 3 kelimeli: "long double X" (2 kelime tip + isim) */
			else if (words[k] == "long" && k + 2 < wc && words[k + 1] == "double") {
				if (is_valid_identifier(words[k + 2])) {
					r_names.push_back(words[k + 2]);
				}
				is_type = true;
			}
			/* 2 kelimeli: "unsigned short|int|long X" */
			else if (words[k] == "unsigned" && k + 2 < wc &&
					(words[k + 1] == "short" || words[k + 1] == "int" || words[k + 1] == "long")) {
				if (is_valid_identifier(words[k + 2])) {
					r_names.push_back(words[k + 2]);
				}
				is_type = true;
			}
			/* 2 kelimeli: "long long X" */
			else if (words[k] == "long" && k + 2 < wc && words[k + 1] == "long") {
				if (is_valid_identifier(words[k + 2])) {
					r_names.push_back(words[k + 2]);
				}
				is_type = true;
			}

			if (is_type) {
				continue;
			}

			if (is_number_keyword(words[k])) {
				/* Takip eden kelime de bir tip kelimesiyse bu kelime cok
				   kelimeli tipin parcasi demektir ("long int X" gibi):
				   degisken adi daha sonra gelir. */
				if (k + 1 < wc && is_number_keyword(words[k + 1])) {
					continue;
				}
				String cleaned = clean_identifier(words[k + 1]);
				if (k + 1 < wc && is_valid_identifier(cleaned)) {
					r_names.push_back(cleaned);
				}
			}
		}

		if (line_end == len) {
			break;
		}
		line_start = line_end + 1;
	}
}

} // namespace gcl
