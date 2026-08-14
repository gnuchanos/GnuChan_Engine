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

/* Bir satir fonksiyon imzasi mi? ("void Update(float delta) {") amac: imza
   satirindaki parametreler GLOBAL degiskene donusmesin. Parametreler
   function_params_scan ile scoped olarak onerilir. */
static bool is_function_signature_line(const String &p_line) {
	int paren = p_line.find("(");
	if (paren <= 0) {
		return false;
	}
	/* Parantez oncesi 'void Name' kalibini ara (bosluk/sekme birakilabilir). */
	String head = p_line.substr(0, paren).strip_edges();
	if (!head.begins_with("void ") && head != "void") {
		return false;
	}
	int paren_close = p_line.find(")", paren);
	if (paren_close == -1) {
		return false;
	}
	/* Parantez kapandiktan sonra '{' (govde acilisi) gelmeli. */
	String tail = p_line.substr(paren_close + 1);
	if (!tail.strip_edges().begins_with("{")) {
		return false;
	}
	return true;
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

/* Bir metnin son kelimesini dondurur (harf/rakam/_). "float delta" -> "delta". */
static String get_last_word(const String &p_text) {
	int i = p_text.length();
	while (i > 0) {
		CharType c = p_text[i - 1];
		bool is_word = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') || c == '_';
		if (!is_word) {
			break;
		}
		i--;
	}
	return p_text.substr(i, p_text.length() - i);
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

		/* Fonksiyon imza satirini atla: 'void Name(float delta) {' gibi.
		   Parametreler GLOBAL degisken degildir; imlecin bulundugu
		   fonksiyona ait parametreler function_params_scan ile onerilir. */
		if (is_function_signature_line(line)) {
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

		/* Satirda DUZ atama ("=") var mi? "NODE x = ...", "FATHER y = ..."
		   gibi kullanici tanimli tipleri guvenle yakalamak icin sart.
		   "self.Raycast.X == deger" satirlarinda "==" bulunur, bu sart
		   false olur ve yanlis değişken yakalanmaz. */
		bool has_plain_assign = line.find("=") != -1 &&
				line.find("==") == -1 && line.find("!=") == -1;

		for (int k = 0; k < wc; k++) {
			bool is_type = false;

			/* Kullanici tanimli tip: NODE, struct/class/typedef isimleri
			   buyuk harfle baslar. Sadece "Tip Degisken = ..." kalibinda
			   yakala; "self.Raycast.GetBodyName" gibi member zinciri bu
			   satirlarda "==" oldugu icin elenir. */
			if (has_plain_assign && k + 1 < wc &&
					words[k].length() > 0 && words[k][0] >= 'A' && words[k][0] <= 'Z' &&
					is_valid_identifier(words[k + 1])) {
				r_names.push_back(words[k + 1]);
				is_type = true;
			}

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
				/* Satirin SON kelimesi tip kelimesi olabilir ("self.Raycast"
				   gibi bir dizi tip adiyla biten satirlar); words[k + 1]'e
				   sinir kontrolu OLMADAN erismek CowData get() crash'ine
				   (out of bounds) yol aciyordu. */
				if (k + 1 < wc) {
					String cleaned = clean_identifier(words[k + 1]);
					if (is_valid_identifier(cleaned)) {
						r_names.push_back(cleaned);
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

/* Imlecin (p_offset) icinde bulundugu fonksiyonun parametre adlarini toplar.
   Sondan geriye dogru ilk "void Name(...) {" imzasi bulunur: kullanici en
   son acilan fonksiyonun govdesinde yaziyordur. Boylece Update ve
   UpdatePhysics ayni anda delta icerdiginde bile yalnizca GECERLI fonksiyonun
   parametresi onerilir - delta asla 2+ kere cikmaz. */
void function_params_scan(const String &p_code, int p_offset, Vector<String> &r_names) {
	r_names.clear();

	if (p_code.empty() || p_offset < 0) {
		return;
	}
	if (p_offset > p_code.length()) {
		p_offset = p_code.length();
	}

	/* p_offset oncesindeki her imza satirini bul; en sonda kalan sonuncudur. */
	String prefix = p_code.substr(0, p_offset);

	int found_start = -1;
	int found_end = -1;

	int s = 0;
	const int len = prefix.length();
	while (s <= len) {
		int e = prefix.find("\n", s);
		if (e == -1) {
			e = len;
		}

		String line = prefix.substr(s, e - s).strip_edges();
		if (is_function_signature_line(line)) {
			found_start = s;
			found_end = e;
		}

		if (e == len) {
			break;
		}
		s = e + 1;
	}

	if (found_start == -1) {
		return; /* imlecin ustunde hic fonksiyon imzasi yok (global scope) */
	}

	String sig = prefix.substr(found_start, found_end - found_start).strip_edges();

	/* Parantez icini ayir: "void Update(float delta)" -> "float delta" */
	int open = sig.find("(");
	if (open == -1) {
		return;
	}
	int close = sig.find(")", open);
	if (close == -1) {
		close = sig.length();
	}
	String inside = sig.substr(open + 1, close - open - 1).strip_edges();
	if (inside.empty()) {
		return; /* parametresiz fonksiyon */
	}

	/* Virgulle ayir, son kelime her parametrenin adidir.
	   "float delta" -> delta; "int x, float y" -> x, y. */
	int part_start = 0;
	const int ilen = inside.length();
	for (int i = 0; i <= ilen; i++) {
		if (i == ilen || inside[i] == ',') {
			String part = inside.substr(part_start, i - part_start).strip_edges();
			part_start = i + 1;
			if (part.empty()) {
				continue;
			}
			String name = get_last_word(part);
			if (is_valid_identifier(name)) {
				r_names.push_back(name);
			}
		}
	}
}

} // namespace gcl
