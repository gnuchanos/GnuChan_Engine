/**************************************************************************/
/*  function.cpp                                                          */
/**************************************************************************/
/*  GCL Parser - Fonksiyon modulu.                                        */
/*    void Ready()          -> islem fonksiyonu                           */
/*    void Update()         -> her kare                                                 */
/*    void PhysicsUpdate()  -> fizik adimi                                */
/*  "void <Isim> (" kalibini bulur, govdeyi brace eslesmesiyle kapatir.   */
/**************************************************************************/

#include "parser.h"

namespace gcl {

bool function_is_known(const String &p_name) {
	return p_name == "Ready" || p_name == "Update" || p_name == "PhysicsUpdate";
}

/* p_text icinde p_from'dan itibaren "void <isim>(" kalibini ara. */
static bool find_void_decl(const String &p_text, int p_from, String &r_name) {
	const int len = p_text.length();

	int w = p_text.find("void", p_from);
	if (w == -1) {
		return false;
	}

	int k = w + 4;
	if (k >= len || (p_text[k] != ' ' && p_text[k] != '\t')) {
		return false;
	}
	while (k < len && (p_text[k] == ' ' || p_text[k] == '\t')) {
		k++;
	}
	if (k >= len) {
		return false;
	}

	/* isim: harf veya _ ile baslar */
	if (!((p_text[k] >= 'a' && p_text[k] <= 'z') ||
				(p_text[k] >= 'A' && p_text[k] <= 'Z') ||
				p_text[k] == '_')) {
		return false;
	}

	int name_start = k;
	while (k < len &&
			((p_text[k] >= 'a' && p_text[k] <= 'z') ||
					(p_text[k] >= 'A' && p_text[k] <= 'Z') ||
					(p_text[k] >= '0' && p_text[k] <= '9') ||
					p_text[k] == '_')) {
		k++;
	}

	r_name = p_text.substr(name_start, k - name_start);
	return true;
}

/* p_text icinde p_from'dan itibaren hedef karakteri ara. */
static int find_char_from(const String &p_text, CharType p_target, int p_from) {
	const int len = p_text.length();
	for (int i = p_from; i < len; i++) {
		if (p_text[i] == p_target) {
			return i;
		}
	}
	return -1;
}

/* Brace eslesmesi: { bulununca derinligi say, kapanan } pozisyonunu don. */
/* p_start: acilis { in bulundugu karakter. */
static int match_brace(const String &p_text, int p_open_pos) {
	int depth = 1;
	const int len = p_text.length();
	for (int i = p_open_pos + 1; i < len; i++) {
		if (p_text[i] == '{') {
			depth++;
		} else if (p_text[i] == '}') {
			depth--;
			if (depth == 0) {
				return i;
			}
		}
	}
	return -1;
}

bool function_parse(const Vector<Token> &p_tokens, int &r_index, String &r_name, int &r_body_start) {
	const int L = p_tokens.size();

	for (int i = r_index; i < L; i++) {
		const Token &tok = p_tokens[i];
		if (tok.kind != TokenKind::CODE_TEXT) {
			continue;
		}

		String name;
		if (!find_void_decl(tok.text, 0, name)) {
			continue;
		}
		if (!function_is_known(name)) {
			continue;
		}

		/* Isimden sonra gelen acik parantez ve kapali parantez. */
		int name_end = tok.text.find(name) + name.length();
		int paren = find_char_from(tok.text, '(', name_end);
		if (paren == -1) {
			continue;
		}
		int close_paren = find_char_from(tok.text, ')', paren + 1);
		if (close_paren == -1) {
			continue;
		}

		/* { acilisini ara: once ayni token, sonra sonraki token'lar. */
		int open_brace = find_char_from(tok.text, '{', close_paren + 1);
		if (open_brace == -1) {
			continue; /* "void Ready()" tek basina: govde baska satirda diye devam et */
		}

		/* Govdeyi kapat: { konumundan derinlik say. */
		int close_brace = match_brace(tok.text, open_brace);
		if (close_brace == -1) {
			continue;
		}

		r_name = name;
		r_body_start = i; /* govde bu token'da baslar */
		r_index = i + 1; /* sonraki parse bu token'dan sonra devam etsin */
		return true;
	}

	return false;
}

} // namespace gcl
