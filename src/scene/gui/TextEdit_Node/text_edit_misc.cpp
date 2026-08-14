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

String TextEdit::get_word_at_pos(const Vector2 &p_pos) const {
	int row, col;
	_get_mouse_pos(p_pos, row, col);

	String s = text[row];
	if (s.length() == 0) {
		return "";
	}
	int beg, end;
	if (select_word(s, col, beg, end)) {
		bool inside_quotes = false;
		CharType selected_quote = '\0';
		int qbegin = 0, qend = 0;
		for (int i = 0; i < s.length(); i++) {
			if (s[i] == '"' || s[i] == '\'') {
				if (i == 0 || s[i - 1] != '\\') {
					if (inside_quotes && selected_quote == s[i]) {
						qend = i;
						inside_quotes = false;
						selected_quote = '\0';
						if (col >= qbegin && col <= qend) {
							return s.substr(qbegin, qend - qbegin);
						}
					} else if (!inside_quotes) {
						qbegin = i + 1;
						inside_quotes = true;
						selected_quote = s[i];
					}
				}
			}
		}

		return s.substr(beg, end - beg);
	}

	return String();
}

String TextEdit::get_tooltip(const Point2 &p_pos) const {
	Object *tooltip_obj = ObjectDB::get_instance(tooltip_obj_id);
	if (!tooltip_obj) {
		return Control::get_tooltip(p_pos);
	}
	int row, col;
	_get_mouse_pos(p_pos, row, col);

	String s = text[row];
	if (s.length() == 0) {
		return Control::get_tooltip(p_pos);
	}
	int beg, end;
	if (select_word(s, col, beg, end)) {
		String tt = tooltip_obj->call(tooltip_func, s.substr(beg, end - beg), tooltip_ud);

		return tt;
	}

	return Control::get_tooltip(p_pos);
}

void TextEdit::set_tooltip_request_func(Object *p_obj, const StringName &p_function, const Variant &p_udata) {
	ERR_FAIL_NULL(p_obj);
	tooltip_obj_id = p_obj->get_instance_id();
	tooltip_func = p_function;
	tooltip_ud = p_udata;
}

void TextEdit::set_line(int line, String new_text) {
	if (line < 0 || line >= text.size()) {
		return;
	}
	_remove_text(line, 0, line, text[line].length());
	_insert_text(line, 0, new_text);
	if (cursor.line == line) {
		cursor.column = MIN(cursor.column, new_text.length());
	}
	if (is_selection_active() && line == selection.to_line && selection.to_column > text[line].length()) {
		selection.to_column = text[line].length();
	}
}

void TextEdit::insert_at(const String &p_text, int at) {
	_insert_text(at, 0, p_text + "\n");
	if (cursor.line >= at) {
		// offset cursor when located after inserted line
		++cursor.line;
	}
	if (is_selection_active()) {
		if (selection.from_line >= at) {
			// offset selection when located after inserted line
			++selection.from_line;
			++selection.to_line;
		} else if (selection.to_line >= at) {
			// extend selection that includes inserted line
			++selection.to_line;
		}
	}
}

void TextEdit::set_show_line_numbers(bool p_show) {
	line_numbers = p_show;
	update();
}

void TextEdit::set_line_numbers_zero_padded(bool p_zero_padded) {
	line_numbers_zero_padded = p_zero_padded;
	update();
}

bool TextEdit::is_show_line_numbers_enabled() const {
	return line_numbers;
}

void TextEdit::set_show_line_length_guidelines(bool p_show) {
	line_length_guidelines = p_show;
	update();
}

void TextEdit::set_line_length_guideline_soft_column(int p_column) {
	line_length_guideline_soft_col = p_column;
	update();
}

void TextEdit::set_line_length_guideline_hard_column(int p_column) {
	line_length_guideline_hard_col = p_column;
	update();
}

void TextEdit::set_bookmark_gutter_enabled(bool p_draw) {
	draw_bookmark_gutter = p_draw;
	update();
}

bool TextEdit::is_bookmark_gutter_enabled() const {
	return draw_bookmark_gutter;
}

void TextEdit::set_breakpoint_gutter_enabled(bool p_draw) {
	draw_breakpoint_gutter = p_draw;
	update();
}

bool TextEdit::is_breakpoint_gutter_enabled() const {
	return draw_breakpoint_gutter;
}

void TextEdit::set_breakpoint_gutter_width(int p_gutter_width) {
	breakpoint_gutter_width = p_gutter_width;
	update();
}

int TextEdit::get_breakpoint_gutter_width() const {
	return cache.breakpoint_gutter_width;
}

void TextEdit::set_draw_fold_gutter(bool p_draw) {
	draw_fold_gutter = p_draw;
	update();
}

bool TextEdit::is_drawing_fold_gutter() const {
	return draw_fold_gutter;
}

void TextEdit::set_fold_gutter_width(int p_gutter_width) {
	fold_gutter_width = p_gutter_width;
	update();
}

int TextEdit::get_fold_gutter_width() const {
	return cache.fold_gutter_width;
}

void TextEdit::set_draw_info_gutter(bool p_draw) {
	draw_info_gutter = p_draw;
	update();
}

bool TextEdit::is_drawing_info_gutter() const {
	return draw_info_gutter;
}

void TextEdit::set_info_gutter_width(int p_gutter_width) {
	info_gutter_width = p_gutter_width;
	update();
}

int TextEdit::get_info_gutter_width() const {
	return info_gutter_width;
}

int TextEdit::get_total_gutter_width() const {
	return cache.line_number_w + cache.breakpoint_gutter_width + cache.fold_gutter_width + cache.info_gutter_width;
}

void TextEdit::set_draw_minimap(bool p_draw) {
	draw_minimap = p_draw;
	update();
}

bool TextEdit::is_drawing_minimap() const {
	return draw_minimap;
}

void TextEdit::set_minimap_width(int p_minimap_width) {
	minimap_width = p_minimap_width;
	update();
}

int TextEdit::get_minimap_width() const {
	return minimap_width;
}

void TextEdit::set_hiding_enabled(bool p_enabled) {
	if (!p_enabled) {
		unhide_all_lines();
	}
	hiding_enabled = p_enabled;
	update();
}

bool TextEdit::is_hiding_enabled() const {
	return hiding_enabled;
}

void TextEdit::set_highlight_current_line(bool p_enabled) {
	highlight_current_line = p_enabled;
	update();
}

bool TextEdit::is_highlight_current_line_enabled() const {
	return highlight_current_line;
}

bool TextEdit::is_text_field() const {
	return true;
}

void TextEdit::menu_option(int p_option) {
	switch (p_option) {
		case MENU_CUT: {
			if (!readonly) {
				cut();
			}
		} break;
		case MENU_COPY: {
			copy();
		} break;
		case MENU_PASTE: {
			if (!readonly) {
				paste();
			}
		} break;
		case MENU_CLEAR: {
			if (!readonly) {
				clear();
			}
		} break;
		case MENU_SELECT_ALL: {
			select_all();
		} break;
		case MENU_UNDO: {
			undo();
		} break;
		case MENU_REDO: {
			redo();
		}
	}
}

void TextEdit::set_select_identifiers_on_hover(bool p_enable) {
	select_identifiers_enabled = p_enable;
}

bool TextEdit::is_selecting_identifiers_on_hover_enabled() const {
	return select_identifiers_enabled;
}

void TextEdit::set_context_menu_enabled(bool p_enable) {
	context_menu_enabled = p_enable;
}

bool TextEdit::is_context_menu_enabled() {
	return context_menu_enabled;
}

void TextEdit::set_shortcut_keys_enabled(bool p_enabled) {
	shortcut_keys_enabled = p_enabled;

	_generate_context_menu();
}

void TextEdit::set_virtual_keyboard_enabled(bool p_enable) {
	virtual_keyboard_enabled = p_enable;
}

void TextEdit::set_middle_mouse_paste_enabled(bool p_enabled) {
	middle_mouse_paste_enabled = p_enabled;
}

void TextEdit::set_selecting_enabled(bool p_enabled) {
	selecting_enabled = p_enabled;

	if (!selecting_enabled) {
		deselect();
	}

	_generate_context_menu();
}

bool TextEdit::is_selecting_enabled() const {
	return selecting_enabled;
}

void TextEdit::set_deselect_on_focus_loss_enabled(const bool p_enabled) {
	deselect_on_focus_loss_enabled = p_enabled;
	if (p_enabled && selection.active && !has_focus()) {
		deselect();
	}
}

bool TextEdit::is_deselect_on_focus_loss_enabled() const {
	return deselect_on_focus_loss_enabled;
}

void TextEdit::set_drag_and_drop_selection_enabled(const bool p_enabled) {
	drag_and_drop_selection_enabled = p_enabled;
}

bool TextEdit::is_drag_and_drop_selection_enabled() const {
	return drag_and_drop_selection_enabled;
}

bool TextEdit::is_shortcut_keys_enabled() const {
	return shortcut_keys_enabled;
}

bool TextEdit::is_virtual_keyboard_enabled() const {
	return virtual_keyboard_enabled;
}

bool TextEdit::is_middle_mouse_paste_enabled() const {
	return middle_mouse_paste_enabled;
}

PopupMenu *TextEdit::get_menu() const {
	return menu;
}

int TextEdit::get_line_width(int p_line, int p_wrap_index) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), 0);

	if (p_wrap_index >= 0 && line_wraps(p_line)) {
		Vector<String> rows = get_wrap_rows_text(p_line);
		ERR_FAIL_INDEX_V(p_wrap_index, rows.size(), 0);

		int w = 0;
		int len = rows[p_wrap_index].length();
		const CharType *str = rows[p_wrap_index].c_str();
		for (int i = 0; i < len; i++) {
			w += text.get_char_width(str[i], str[i + 1], w);
		}

		return w;
	}

	return text.get_line_width(p_line);
}

int TextEdit::get_line_height() const {
	return get_row_height();
}

