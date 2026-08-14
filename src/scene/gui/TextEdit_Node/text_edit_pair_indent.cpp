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


void TextEdit::_consume_pair_symbol(CharType ch) {
	int cursor_position_to_move = cursor_get_column() + 1;

	CharType ch_single[2] = { ch, 0 };
	CharType ch_single_pair[2] = { _get_right_pair_symbol(ch), 0 };
	CharType ch_pair[3] = { ch, _get_right_pair_symbol(ch), 0 };

	if (is_selection_active()) {
		int new_column, new_line;

		begin_complex_operation();
		_insert_text(get_selection_from_line(), get_selection_from_column(),
				ch_single,
				&new_line, &new_column);

		int to_col_offset = 0;
		if (get_selection_from_line() == get_selection_to_line()) {
			to_col_offset = 1;
		}

		_insert_text(get_selection_to_line(),
				get_selection_to_column() + to_col_offset,
				ch_single_pair,
				&new_line, &new_column);
		end_complex_operation();

		cursor_set_line(get_selection_to_line());
		cursor_set_column(get_selection_to_column() + to_col_offset);

		deselect();
		update();
		return;
	}

	if ((ch == '\'' || ch == '"') &&
			cursor_get_column() > 0 && _is_text_char(text[cursor.line][cursor_get_column() - 1]) && !_is_pair_right_symbol(text[cursor.line][cursor_get_column()])) {
		insert_text_at_cursor(ch_single);
		cursor_set_column(cursor_position_to_move);
		return;
	}

	if (cursor_get_column() < text[cursor.line].length()) {
		if (_is_text_char(text[cursor.line][cursor_get_column()])) {
			insert_text_at_cursor(ch_single);
			cursor_set_column(cursor_position_to_move);
			return;
		}
		if (_is_pair_right_symbol(ch) &&
				text[cursor.line][cursor_get_column()] == ch) {
			cursor_set_column(cursor_position_to_move);
			return;
		}
	}

	String line = text[cursor.line];

	bool in_single_quote = false;
	bool in_double_quote = false;
	bool found_comment = false;

	int c = 0;
	while (c < line.length()) {
		if (line[c] == '\\') {
			c++; // Skip quoted anything.

			if (cursor.column == c) {
				break;
			}
		} else if (!in_single_quote && !in_double_quote && line[c] == '#') {
			found_comment = true;
			break;
		} else {
			if (line[c] == '\'' && !in_double_quote) {
				in_single_quote = !in_single_quote;
			} else if (line[c] == '"' && !in_single_quote) {
				in_double_quote = !in_double_quote;
			}
		}

		c++;

		if (cursor.column == c) {
			break;
		}
	}

	// Do not need to duplicate quotes while in comments
	if (found_comment) {
		insert_text_at_cursor(ch_single);
		cursor_set_column(cursor_position_to_move);

		return;
	}

	// Disallow inserting duplicated quotes while already in string
	if ((in_single_quote || in_double_quote) && (ch == '"' || ch == '\'')) {
		insert_text_at_cursor(ch_single);
		cursor_set_column(cursor_position_to_move);

		return;
	}

	insert_text_at_cursor(ch_pair);
	cursor_set_column(cursor_position_to_move);
}

void TextEdit::_consume_backspace_for_pair_symbol(int prev_line, int prev_column) {
	bool remove_right_symbol = false;

	if (cursor.column < text[cursor.line].length() && cursor.column > 0) {
		CharType left_char = text[cursor.line][cursor.column - 1];
		CharType right_char = text[cursor.line][cursor.column];

		if (right_char == _get_right_pair_symbol(left_char)) {
			remove_right_symbol = true;
		}
	}
	if (remove_right_symbol) {
		_remove_text(prev_line, prev_column, cursor.line, cursor.column + 1);
	} else {
		_remove_text(prev_line, prev_column, cursor.line, cursor.column);
	}
}
void TextEdit::backspace_at_cursor() {
	if (readonly) {
		return;
	}

	if (cursor.column == 0 && cursor.line == 0) {
		return;
	}

	int prev_line = cursor.column ? cursor.line : cursor.line - 1;
	int prev_column = cursor.column ? (cursor.column - 1) : (text[cursor.line - 1].length());

	if (is_line_hidden(cursor.line)) {
		set_line_as_hidden(prev_line, true);
	}
	if (is_line_set_as_breakpoint(cursor.line)) {
		if (!text.is_breakpoint(prev_line)) {
			emit_signal("breakpoint_toggled", prev_line);
		}
		set_line_as_breakpoint(prev_line, true);
	}

	if (text.has_info_icon(cursor.line)) {
		set_line_info_icon(prev_line, text.get_info_icon(cursor.line), text.get_info(cursor.line));
	}

	if (auto_brace_completion_enabled &&
			cursor.column > 0 &&
			_is_pair_left_symbol(text[cursor.line][cursor.column - 1])) {
		_consume_backspace_for_pair_symbol(prev_line, prev_column);
	} else {
		// Handle space indentation.
		if (cursor.column != 0 && indent_using_spaces) {
			// Check if there are no other chars before cursor, just indentation.
			bool unindent = true;
			int i = 0;
			while (i < cursor.column && i < text[cursor.line].length()) {
				if (!_is_whitespace(text[cursor.line][i])) {
					unindent = false;
					break;
				}
				i++;
			}

			// Then we can remove all spaces as a single character.
			if (unindent) {
				// We want to remove spaces up to closest indent, or whole indent if cursor is pointing at it.
				int spaces_to_delete = _calculate_spaces_till_next_left_indent(cursor.column);
				prev_column = cursor.column - spaces_to_delete;
				_remove_text(cursor.line, prev_column, cursor.line, cursor.column);
			} else {
				_remove_text(prev_line, prev_column, cursor.line, cursor.column);
			}
		} else {
			_remove_text(prev_line, prev_column, cursor.line, cursor.column);
		}
	}

	cursor_set_line(prev_line, false, true);
	cursor_set_column(prev_column);
}

void TextEdit::indent_right() {
	int start_line;
	int end_line;

	// This value informs us by how much we changed selection position by indenting right.
	// Default is 1 for tab indentation.
	int selection_offset = 1;
	begin_complex_operation();

	if (is_selection_active()) {
		start_line = get_selection_from_line();
		end_line = get_selection_to_line();
	} else {
		start_line = cursor.line;
		end_line = start_line;
	}

	// Ignore if the cursor is not past the first column.
	if (is_selection_active() && get_selection_to_column() == 0) {
		selection_offset = 0;
		end_line--;
	}

	for (int i = start_line; i <= end_line; i++) {
		String line_text = get_line(i);
		if (line_text.size() == 0 && is_selection_active()) {
			continue;
		}
		if (indent_using_spaces) {
			// We don't really care where selection is - we just need to know indentation level at the beginning of the line.
			int left = _find_first_non_whitespace_column_of_line(line_text);
			int spaces_to_add = _calculate_spaces_till_next_right_indent(left);
			// Since we will add this much spaces we want move whole selection and cursor by this much.
			selection_offset = spaces_to_add;
			for (int j = 0; j < spaces_to_add; j++) {
				line_text = ' ' + line_text;
			}
		} else {
			line_text = '\t' + line_text;
		}
		set_line(i, line_text);
	}

	// Fix selection and cursor being off after shifting selection right.
	if (is_selection_active()) {
		select(selection.from_line, selection.from_column + selection_offset, selection.to_line, selection.to_column + selection_offset);
	}
	cursor_set_column(cursor.column + selection_offset, false);
	end_complex_operation();
	update();
}

void TextEdit::indent_left() {
	int start_line;
	int end_line;

	// Moving cursor and selection after unindenting can get tricky because
	// changing content of line can move cursor and selection on it's own (if new line ends before previous position of either),
	// therefore we just remember initial values and at the end of the operation offset them by number of removed characters.
	int removed_characters = 0;
	int initial_selection_end_column = selection.to_column;
	int initial_cursor_column = cursor.column;

	begin_complex_operation();

	if (is_selection_active()) {
		start_line = get_selection_from_line();
		end_line = get_selection_to_line();
	} else {
		start_line = cursor.line;
		end_line = start_line;
	}

	// Ignore if the cursor is not past the first column.
	if (is_selection_active() && get_selection_to_column() == 0) {
		end_line--;
	}
	String first_line_text = get_line(start_line);
	String last_line_text = get_line(end_line);

	for (int i = start_line; i <= end_line; i++) {
		String line_text = get_line(i);

		if (line_text.begins_with("\t")) {
			line_text = line_text.substr(1, line_text.length());
			set_line(i, line_text);
			removed_characters = 1;
		} else if (line_text.begins_with(" ")) {
			// When unindenting we aim to remove spaces before line that has selection no matter what is selected,
			// so we start of by finding first non whitespace character of line
			int left = _find_first_non_whitespace_column_of_line(line_text);

			// Here we remove only enough spaces to align text to nearest full multiple of indentation_size.
			// In case where selection begins at the start of indentation_size multiple we remove whole indentation level.
			int spaces_to_remove = _calculate_spaces_till_next_left_indent(left);

			line_text = line_text.substr(spaces_to_remove, line_text.length());
			set_line(i, line_text);
			removed_characters = spaces_to_remove;
		}
	}

	if (is_selection_active()) {
		// Fix selection being off by one on the first line.
		if (first_line_text != get_line(start_line)) {
			select(selection.from_line, selection.from_column - removed_characters,
					selection.to_line, initial_selection_end_column);
		}
		// Fix selection being off by one on the last line.
		if (last_line_text != get_line(end_line)) {
			select(selection.from_line, selection.from_column,
					selection.to_line, initial_selection_end_column - removed_characters);
		}
	}
	cursor_set_column(initial_cursor_column - removed_characters, false);
	end_complex_operation();
	update();
}

int TextEdit::_calculate_spaces_till_next_left_indent(int column) {
	int spaces_till_indent = column % indent_size;
	if (spaces_till_indent == 0) {
		spaces_till_indent = indent_size;
	}
	return spaces_till_indent;
}

int TextEdit::_calculate_spaces_till_next_right_indent(int column) {
	return indent_size - column % indent_size;
}

void TextEdit::_get_mouse_pos(const Point2i &p_mouse, int &r_row, int &r_col) const {
	float rows = p_mouse.y;
	rows -= cache.style_normal->get_margin(MARGIN_TOP);
	rows /= get_row_height();
	rows += get_v_scroll_offset();
	int first_vis_line = get_first_visible_line();
	int row = first_vis_line + Math::floor(rows);
	int wrap_index = 0;

	if (is_wrap_enabled() || is_hiding_enabled()) {
		int f_ofs = num_lines_from_rows(first_vis_line, cursor.wrap_ofs, rows + (1 * SGN(rows)), wrap_index) - 1;
		if (rows < 0) {
			row = first_vis_line - f_ofs;
		} else {
			row = first_vis_line + f_ofs;
		}
	}

	if (row < 0) {
		row = 0; // TODO.
	}

	int col = 0;

	if (row >= text.size()) {
		row = text.size() - 1;
		col = text[row].size();
	} else {
		int colx = p_mouse.x - (cache.style_normal->get_margin(MARGIN_LEFT) + cache.line_number_w + cache.breakpoint_gutter_width + cache.fold_gutter_width + cache.info_gutter_width);
		colx += cursor.x_ofs;
		col = get_char_pos_for_line(colx, row, wrap_index);
		if (is_wrap_enabled() && wrap_index < times_line_wraps(row)) {
			// Move back one if we are at the end of the row.
			Vector<String> rows2 = get_wrap_rows_text(row);
			int row_end_col = 0;
			for (int i = 0; i < wrap_index + 1; i++) {
				row_end_col += rows2[i].length();
			}
			if (col >= row_end_col) {
				col -= 1;
			}
		}
	}

	r_row = row;
	r_col = col;
}

Vector2i TextEdit::_get_cursor_pixel_pos() {
	adjust_viewport_to_cursor();
	int row = (cursor.line - get_first_visible_line() - cursor.wrap_ofs);
	// Correct for hidden and wrapped lines
	for (int i = get_first_visible_line(); i < cursor.line; i++) {
		if (is_line_hidden(i)) {
			row -= 1;
			continue;
		}
		row += times_line_wraps(i);
	}
	// Row might be wrapped. Adjust row and r_column
	Vector<String> rows2 = get_wrap_rows_text(cursor.line);
	while (rows2.size() > 1) {
		if (cursor.column >= rows2[0].length()) {
			cursor.column -= rows2[0].length();
			rows2.remove(0);
			row++;
		} else {
			break;
		}
	}

	// Calculate final pixel position
	int y = (row - get_v_scroll_offset() + 1 /*Bottom of line*/) * get_row_height();
	int x = cache.style_normal->get_margin(MARGIN_LEFT) + cache.line_number_w + cache.breakpoint_gutter_width + cache.fold_gutter_width + cache.info_gutter_width - cursor.x_ofs;
	int ix = 0;
	while (ix < rows2[0].size() && ix < cursor.column) {
		if (cache.font != nullptr) {
			x += cache.font->get_char_size(rows2[0].get(ix)).width;
		}
		ix++;
	}
	x += get_indent_level(cursor.line) * cache.font->get_char_size(' ').width;

	return Vector2i(x, y);
}

void TextEdit::_get_minimap_mouse_row(const Point2i &p_mouse, int &r_row) const {
	float rows = p_mouse.y;
	rows -= cache.style_normal->get_margin(MARGIN_TOP);
	rows /= (minimap_char_size.y + minimap_line_spacing);
	rows += get_v_scroll_offset();

	// calculate visible lines
	int minimap_visible_lines = _get_minimap_visible_rows();
	int visible_rows = get_visible_rows() + 1;
	int first_visible_line = get_first_visible_line() - 1;
	int draw_amount = visible_rows + (smooth_scroll_enabled ? 1 : 0);
	draw_amount += times_line_wraps(first_visible_line + 1);
	int minimap_line_height = (minimap_char_size.y + minimap_line_spacing);

	// calculate viewport size and y offset
	int viewport_height = (draw_amount - 1) * minimap_line_height;
	int control_height = _get_control_height() - viewport_height;
	int viewport_offset_y = round(get_scroll_pos_for_line(first_visible_line) * control_height) / ((v_scroll->get_max() <= minimap_visible_lines) ? (minimap_visible_lines - draw_amount) : (v_scroll->get_max() - draw_amount));

	// calculate the first line.
	int num_lines_before = round((viewport_offset_y) / minimap_line_height);
	int wi;
	int minimap_line = (v_scroll->get_max() <= minimap_visible_lines) ? -1 : first_visible_line;
	if (first_visible_line > 0 && minimap_line >= 0) {
		minimap_line -= num_lines_from_rows(first_visible_line, 0, -num_lines_before, wi);
		minimap_line -= (minimap_line > 0 && smooth_scroll_enabled ? 1 : 0);
	} else {
		minimap_line = 0;
	}

	int row = minimap_line + Math::floor(rows);
	int wrap_index = 0;

	if (is_wrap_enabled() || is_hiding_enabled()) {
		int f_ofs = num_lines_from_rows(minimap_line, cursor.wrap_ofs, rows + (1 * SGN(rows)), wrap_index) - 1;
		if (rows < 0) {
			row = minimap_line - f_ofs;
		} else {
			row = minimap_line + f_ofs;
		}
	}

	if (row < 0) {
		row = 0;
	}

	if (row >= text.size()) {
		row = text.size() - 1;
	}

	r_row = row;
}