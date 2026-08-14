#ifndef TEXT_EDIT_HELPERS_H
#define TEXT_EDIT_HELPERS_H

#include "text_edit.h"

#define TAB_PIXELS

/* Tek sorumluluk: TextEdit'in tum modullerinin ortak kullandigi
 * satir icinde (static inline) yardimci fonksiyonlar. Tek tanim,
 * her TU kendi kopyasini uretir -> ODR sorunu olmaz. */

static inline bool _is_symbol(CharType c) {
	return is_symbol(c);
}

static inline bool _is_text_char(CharType c) {
	return !is_symbol(c);
}

static inline bool _is_whitespace(CharType c) {
	return c == '\t' || c == ' ';
}

static inline bool _is_char(CharType c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline bool _is_number(CharType c) {
	return (c >= '0' && c <= '9');
}

static inline bool _is_hex_symbol(CharType c) {
	return ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

static inline bool _is_pair_right_symbol(CharType c) {
	return c == '"' || c == '\'' || c == ')' || c == ']' || c == '}';
}

static inline bool _is_pair_left_symbol(CharType c) {
	return c == '"' || c == '\'' || c == '(' || c == '[' || c == '{';
}

static inline bool _is_pair_symbol(CharType c) {
	return _is_pair_left_symbol(c) || _is_pair_right_symbol(c);
}

static inline CharType _get_right_pair_symbol(CharType c) {
	if (c == '"') return '"';
	if (c == '\'') return '\'';
	if (c == '(') return ')';
	if (c == '[') return ']';
	if (c == '{') return '}';
	return 0;
}

static inline int _find_first_non_whitespace_column_of_line(const String &line) {
	int left = 0;
	while (left < line.length() && _is_whitespace(line[left])) {
		left++;
	}
	return left;
}

/* NOT: _is_completable burada YOK — orijinal kaynakta static olarak
 * text_edit_completion.cpp icinde tanimlidir; iki tanim cakismasini
 * onlemek icin single-definition orada durur. GCL icin nokta (.) sonrasi
 * tetikleme duzeltmesi o dosyada yapilir. */

#endif // TEXT_EDIT_HELPERS_H
