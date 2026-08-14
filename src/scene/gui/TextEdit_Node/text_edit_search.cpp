/**************************************************************************/
/*  GnuChan Engine — TextEdit modular sources                             */
/*                                                                        */
/*  Godot 3.6 text_edit.cpp tek sorumluluk modullerine bolunmustur:        */
/*    text_edit_text.cpp        - Text veri modeli + satir lambasi         */
/*    text_edit_selection.cpp   - secim + minimap etkilesimi               */
/*    text_edit_draw.cpp        - _notification + cizim                    */
/*    text_edit_pair_indent.cpp - cift sembol / backspace / indent         */
/*    text_edit_input.cpp       - _gui_input (klavye + fare)               */
/*    text_edit_core.cpp        - scroll, imlec, wrap, giris metni         */
/*    text_edit_position.cpp    - konum hesaplama + drag&drop + text API   */
/*    text_edit_syntax.cpp      - syntax renklendirme + kes/kopyala/sec    */
/*    text_edit_search.cpp      - arama + satir isaretleri                 */
/*    text_edit_fold_undo.cpp   - katlama + undo/redo                      */
/*    text_edit_completion.cpp  - OTOMATIK TAMAMLAMA (GCL kancali)         */
/*    text_edit_misc.cpp        - tooltip / word / menu / gutter setter'lar*/
/*    text_edit_bind.cpp        - _bind_methods + kurucu + yikici          */
/**************************************************************************/

#include "text_edit.h"
#include "text_edit_helpers.h"

#include "core/message_queue.h"
#include "core/os/input.h"
#include "core/os/keyboard.h"
#include "core/os/os.h"
#include "core/project_settings.h"
#include "core/script_language.h"
#include "label.h"
#include "scene/main/viewport.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_scale.h"
#endif

String TextEdit::get_selection_text() const {
	if (!selection.active) {
		return "";
	}

	return _base_get_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
}

String TextEdit::get_word_under_cursor() const {
	int prev_cc = cursor.column;
	while (prev_cc > 0) {
		bool is_char = _is_text_char(text[cursor.line][prev_cc - 1]);
		if (!is_char) {
			break;
		}
		--prev_cc;
	}

	int next_cc = cursor.column;
	while (next_cc < text[cursor.line].length()) {
		bool is_char = _is_text_char(text[cursor.line][next_cc]);
		if (!is_char) {
			break;
		}
		++next_cc;
	}
	if (prev_cc == cursor.column || next_cc == cursor.column) {
		return "";
	}
	return text[cursor.line].substr(prev_cc, next_cc - prev_cc);
}

void TextEdit::set_search_text(const String &p_search_text) {
	search_text = p_search_text;
}

void TextEdit::set_search_flags(uint32_t p_flags) {
	search_flags = p_flags;
}

void TextEdit::set_current_search_result(int line, int col) {
	search_result_line = line;
	search_result_col = col;
	update();
}

void TextEdit::set_highlight_all_occurrences(const bool p_enabled) {
	highlight_all_occurrences = p_enabled;
	update();
}

bool TextEdit::is_highlight_all_occurrences_enabled() const {
	return highlight_all_occurrences;
}

int TextEdit::_get_column_pos_of_word(const String &p_key, const String &p_search, uint32_t p_search_flags, int p_from_column) {
	int col = -1;

	if (p_key.length() > 0 && p_search.length() > 0) {
		if (p_from_column < 0 || p_from_column > p_search.length()) {
			p_from_column = 0;
		}

		while (col == -1 && p_from_column <= p_search.length()) {
			if (p_search_flags & SEARCH_MATCH_CASE) {
				col = p_search.find(p_key, p_from_column);
			} else {
				col = p_search.findn(p_key, p_from_column);
			}

			// Whole words only.
			if (col != -1 && p_search_flags & SEARCH_WHOLE_WORDS) {
				p_from_column = col;

				if (col > 0 && _is_text_char(p_search[col - 1])) {
					col = -1;
				} else if ((col + p_key.length()) < p_search.length() && _is_text_char(p_search[col + p_key.length()])) {
					col = -1;
				}
			}

			p_from_column += 1;
		}
	}
	return col;
}

PoolVector<int> TextEdit::_search_bind(const String &p_key, uint32_t p_search_flags, int p_from_line, int p_from_column) const {
	int col, line;
	if (search(p_key, p_search_flags, p_from_line, p_from_column, line, col)) {
		PoolVector<int> result;
		result.resize(2);
		result.set(SEARCH_RESULT_COLUMN, col);
		result.set(SEARCH_RESULT_LINE, line);
		return result;

	} else {
		return PoolVector<int>();
	}
}

bool TextEdit::search(const String &p_key, uint32_t p_search_flags, int p_from_line, int p_from_column, int &r_line, int &r_column) const {
	if (p_key.length() == 0) {
		return false;
	}
	ERR_FAIL_INDEX_V(p_from_line, text.size(), false);
	ERR_FAIL_INDEX_V(p_from_column, text[p_from_line].length() + 1, false);

	// Search through the whole document, but start by current line.

	int line = p_from_line;
	int pos = -1;

	for (int i = 0; i < text.size() + 1; i++) {
		if (line < 0) {
			line = text.size() - 1;
		}
		if (line == text.size()) {
			line = 0;
		}

		String text_line = text[line];
		int from_column = 0;
		if (line == p_from_line) {
			if (i == text.size()) {
				// Wrapped.

				if (p_search_flags & SEARCH_BACKWARDS) {
					from_column = text_line.length();
				} else {
					from_column = 0;
				}

			} else {
				from_column = p_from_column;
			}

		} else {
			if (p_search_flags & SEARCH_BACKWARDS) {
				from_column = text_line.length() - 1;
			} else {
				from_column = 0;
			}
		}

		pos = -1;

		int pos_from = (p_search_flags & SEARCH_BACKWARDS) ? text_line.length() : 0;
		int last_pos = -1;

		while (true) {
			if (p_search_flags & SEARCH_BACKWARDS) {
				while ((last_pos = (p_search_flags & SEARCH_MATCH_CASE) ? text_line.rfind(p_key, pos_from) : text_line.rfindn(p_key, pos_from)) != -1) {
					if (last_pos <= from_column) {
						pos = last_pos;
						break;
					}
					pos_from = last_pos - p_key.length();
					if (pos_from < 0) {
						break;
					}
				}
			} else {
				while ((last_pos = (p_search_flags & SEARCH_MATCH_CASE) ? text_line.find(p_key, pos_from) : text_line.findn(p_key, pos_from)) != -1) {
					if (last_pos >= from_column) {
						pos = last_pos;
						break;
					}
					pos_from = last_pos + p_key.length();
				}
			}

			bool is_match = true;

			if (pos != -1 && (p_search_flags & SEARCH_WHOLE_WORDS)) {
				// Validate for whole words.
				if (pos > 0 && _is_text_char(text_line[pos - 1])) {
					is_match = false;
				} else if (pos + p_key.length() < text_line.length() && _is_text_char(text_line[pos + p_key.length()])) {
					is_match = false;
				}
			}

			if (pos_from == -1) {
				pos = -1;
			}

			if (is_match || last_pos == -1 || pos == -1) {
				break;
			}

			pos_from = (p_search_flags & SEARCH_BACKWARDS) ? pos - 1 : pos + 1;
			pos = -1;
		}

		if (pos != -1) {
			break;
		}

		if (p_search_flags & SEARCH_BACKWARDS) {
			line--;
		} else {
			line++;
		}
	}

	if (pos == -1) {
		r_line = -1;
		r_column = -1;
		return false;
	}

	r_line = line;
	r_column = pos;

	return true;
}

void TextEdit::_cursor_changed_emit() {
	emit_signal("cursor_changed");
	cursor_changed_dirty = false;
}

void TextEdit::_text_changed_emit() {
	emit_signal("text_changed");
	text_changed_dirty = false;
}

