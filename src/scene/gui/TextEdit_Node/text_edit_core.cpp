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

void TextEdit::_scroll_up(real_t p_delta) {
	if (scrolling && smooth_scroll_enabled && SGN(target_v_scroll - v_scroll->get_value()) != SGN(-p_delta)) {
		scrolling = false;
		minimap_clicked = false;
	}

	if (scrolling) {
		target_v_scroll = (target_v_scroll - p_delta);
	} else {
		target_v_scroll = (get_v_scroll() - p_delta);
	}

	if (smooth_scroll_enabled) {
		if (target_v_scroll <= 0) {
			target_v_scroll = 0;
		}
		if (Math::abs(target_v_scroll - v_scroll->get_value()) < 1.0) {
			v_scroll->set_value(target_v_scroll);
		} else {
			scrolling = true;
			set_physics_process_internal(true);
		}
	} else {
		set_v_scroll(target_v_scroll);
	}
}

void TextEdit::_scroll_down(real_t p_delta) {
	if (scrolling && smooth_scroll_enabled && SGN(target_v_scroll - v_scroll->get_value()) != SGN(p_delta)) {
		scrolling = false;
		minimap_clicked = false;
	}

	if (scrolling) {
		target_v_scroll = (target_v_scroll + p_delta);
	} else {
		target_v_scroll = (get_v_scroll() + p_delta);
	}

	if (smooth_scroll_enabled) {
		int max_v_scroll = round(v_scroll->get_max() - v_scroll->get_page());
		if (target_v_scroll > max_v_scroll) {
			target_v_scroll = max_v_scroll;
		}
		if (Math::abs(target_v_scroll - v_scroll->get_value()) < 1.0) {
			v_scroll->set_value(target_v_scroll);
		} else {
			scrolling = true;
			set_physics_process_internal(true);
		}
	} else {
		set_v_scroll(target_v_scroll);
	}
}

void TextEdit::_pre_shift_selection() {
	if (!selection.active || selection.selecting_mode == Selection::MODE_NONE) {
		selection.selecting_line = cursor.line;
		selection.selecting_column = cursor.column;
		selection.active = true;
	}

	selection.selecting_mode = Selection::MODE_SHIFT;
}

void TextEdit::_post_shift_selection() {
	if (selection.active && selection.selecting_mode == Selection::MODE_SHIFT) {
		select(selection.selecting_line, selection.selecting_column, cursor.line, cursor.column);
		update();
	}

	selection.selecting_text = true;
}

void TextEdit::_scroll_lines_up() {
	scrolling = false;
	minimap_clicked = false;

	// Adjust the vertical scroll.
	set_v_scroll(get_v_scroll() - 1);

	// Adjust the cursor to viewport.
	if (!selection.active) {
		int cur_line = cursor.line;
		int cur_wrap = get_cursor_wrap_index();
		int last_vis_line = get_last_full_visible_line();
		int last_vis_wrap = get_last_full_visible_line_wrap_index();

		if (cur_line > last_vis_line || (cur_line == last_vis_line && cur_wrap > last_vis_wrap)) {
			cursor_set_line(last_vis_line, false, false, last_vis_wrap);
		}
	}
}

void TextEdit::_scroll_lines_down() {
	scrolling = false;
	minimap_clicked = false;

	// Adjust the vertical scroll.
	set_v_scroll(get_v_scroll() + 1);

	// Adjust the cursor to viewport.
	if (!selection.active) {
		int cur_line = cursor.line;
		int cur_wrap = get_cursor_wrap_index();
		int first_vis_line = get_first_visible_line();
		int first_vis_wrap = cursor.wrap_ofs;

		if (cur_line < first_vis_line || (cur_line == first_vis_line && cur_wrap < first_vis_wrap)) {
			cursor_set_line(first_vis_line, false, false, first_vis_wrap);
		}
	}
}

/**** TEXT EDIT CORE API ****/

void TextEdit::_base_insert_text(int p_line, int p_char, const String &p_text, int &r_end_line, int &r_end_column) {
	// Save for undo.
	ERR_FAIL_INDEX(p_line, text.size());
	ERR_FAIL_COND(p_char < 0);

	/* STEP 1: Remove \r from source text and separate in substrings. */

	Vector<String> substrings = p_text.replace("\r", "").split("\n");

	/* STEP 2: Fire breakpoint_toggled signals. */

	// Is this just a new empty line?
	bool shift_first_line = p_char == 0 && p_text.replace("\r", "") == "\n";

	int i = p_line + !shift_first_line;
	int lines = substrings.size() - 1;
	for (; i < text.size(); i++) {
		if (text.is_breakpoint(i)) {
			if ((i - lines < p_line || !text.is_breakpoint(i - lines)) || (i - lines == p_line && !shift_first_line)) {
				emit_signal("breakpoint_toggled", i);
			}
			if (i + lines >= text.size() || !text.is_breakpoint(i + lines)) {
				emit_signal("breakpoint_toggled", i + lines);
			}
		}
	}

	/* STEP 3: Add spaces if the char is greater than the end of the line. */
	while (p_char > text[p_line].length()) {
		text.set(p_line, text[p_line] + String::chr(' '));
	}

	/* STEP 4: Separate dest string in pre and post text. */

	String preinsert_text = text[p_line].substr(0, p_char);
	String postinsert_text = text[p_line].substr(p_char, text[p_line].size());

	for (int j = 0; j < substrings.size(); j++) {
		// Insert the substrings.

		if (j == 0) {
			text.set(p_line, preinsert_text + substrings[j]);
		} else {
			text.insert(p_line + j, substrings[j]);
		}

		if (j == substrings.size() - 1) {
			text.set(p_line + j, text[p_line + j] + postinsert_text);
		}
	}

	if (shift_first_line) {
		text.set_breakpoint(p_line + 1, text.is_breakpoint(p_line));
		text.set_hidden(p_line + 1, text.is_hidden(p_line));
		if (text.has_info_icon(p_line)) {
			text.set_info_icon(p_line + 1, text.get_info_icon(p_line), text.get_info(p_line));
		}

		text.set_breakpoint(p_line, false);
		text.set_hidden(p_line, false);
		text.set_info_icon(p_line, nullptr, "");
	}

	text.set_line_wrap_amount(p_line, -1);

	r_end_line = p_line + substrings.size() - 1;
	r_end_column = text[r_end_line].length() - postinsert_text.length();

	if (!text_changed_dirty && !setting_text) {
		if (is_inside_tree()) {
			MessageQueue::get_singleton()->push_call(this, "_text_changed_emit");
		}
		text_changed_dirty = true;
	}
	_line_edited_from(p_line);
}

String TextEdit::_base_get_text(int p_from_line, int p_from_column, int p_to_line, int p_to_column) const {
	ERR_FAIL_INDEX_V(p_from_line, text.size(), String());
	ERR_FAIL_INDEX_V(p_from_column, text[p_from_line].length() + 1, String());
	ERR_FAIL_INDEX_V(p_to_line, text.size(), String());
	ERR_FAIL_INDEX_V(p_to_column, text[p_to_line].length() + 1, String());
	ERR_FAIL_COND_V(p_to_line < p_from_line, String()); // 'from > to'.
	ERR_FAIL_COND_V(p_to_line == p_from_line && p_to_column < p_from_column, String()); // 'from > to'.

	String ret;

	for (int i = p_from_line; i <= p_to_line; i++) {
		int begin = (i == p_from_line) ? p_from_column : 0;
		int end = (i == p_to_line) ? p_to_column : text[i].length();

		if (i > p_from_line) {
			ret += "\n";
		}
		ret += text[i].substr(begin, end - begin);
	}

	return ret;
}

void TextEdit::_base_remove_text(int p_from_line, int p_from_column, int p_to_line, int p_to_column) {
	ERR_FAIL_INDEX(p_from_line, text.size());
	ERR_FAIL_INDEX(p_from_column, text[p_from_line].length() + 1);
	ERR_FAIL_INDEX(p_to_line, text.size());
	ERR_FAIL_INDEX(p_to_column, text[p_to_line].length() + 1);
	ERR_FAIL_COND(p_to_line < p_from_line); // 'from > to'.
	ERR_FAIL_COND(p_to_line == p_from_line && p_to_column < p_from_column); // 'from > to'.

	String pre_text = text[p_from_line].substr(0, p_from_column);
	String post_text = text[p_to_line].substr(p_to_column, text[p_to_line].length());

	int lines = p_to_line - p_from_line;

	for (int i = p_from_line + 1; i < text.size(); i++) {
		if (text.is_breakpoint(i)) {
			if (i + lines >= text.size() || !text.is_breakpoint(i + lines)) {
				emit_signal("breakpoint_toggled", i);
			}
			if (i > p_to_line && (i - lines < 0 || !text.is_breakpoint(i - lines))) {
				emit_signal("breakpoint_toggled", i - lines);
			}
		}
	}

	for (int i = p_from_line; i < p_to_line; i++) {
		text.remove(p_from_line + 1);
	}
	text.set(p_from_line, pre_text + post_text);

	text.set_line_wrap_amount(p_from_line, -1);

	if (!text_changed_dirty && !setting_text) {
		if (is_inside_tree()) {
			MessageQueue::get_singleton()->push_call(this, "_text_changed_emit");
		}
		text_changed_dirty = true;
	}
	_line_edited_from(p_from_line);
}

void TextEdit::_insert_text(int p_line, int p_char, const String &p_text, int *r_end_line, int *r_end_char) {
	if (!setting_text && idle_detect->is_inside_tree()) {
		idle_detect->start();
	}

	if (undo_enabled) {
		_clear_redo();
	}

	int retline, retchar;
	_base_insert_text(p_line, p_char, p_text, retline, retchar);
	if (r_end_line) {
		*r_end_line = retline;
	}
	if (r_end_char) {
		*r_end_char = retchar;
	}

	if (!undo_enabled) {
		return;
	}

	/* UNDO!! */
	TextOperation op;
	op.type = TextOperation::TYPE_INSERT;
	op.from_line = p_line;
	op.from_column = p_char;
	op.to_line = retline;
	op.to_column = retchar;
	op.text = p_text;
	op.version = ++version;
	op.chain_forward = false;
	op.chain_backward = false;

	// See if it should just be set as current op.
	if (current_op.type != op.type) {
		op.prev_version = get_version();
		_push_current_op();
		current_op = op;

		return; // Set as current op, return.
	}
	// See if it can be merged.
	if (current_op.to_line != p_line || current_op.to_column != p_char) {
		op.prev_version = get_version();
		_push_current_op();
		current_op = op;
		return; // Set as current op, return.
	}
	// Merge current op.

	current_op.text += p_text;
	current_op.to_column = retchar;
	current_op.to_line = retline;
	current_op.version = op.version;
}

void TextEdit::_remove_text(int p_from_line, int p_from_column, int p_to_line, int p_to_column) {
	if (!setting_text && idle_detect->is_inside_tree()) {
		idle_detect->start();
	}

	String text;
	if (undo_enabled) {
		_clear_redo();
		text = _base_get_text(p_from_line, p_from_column, p_to_line, p_to_column);
	}

	_base_remove_text(p_from_line, p_from_column, p_to_line, p_to_column);

	if (!undo_enabled) {
		return;
	}

	/* UNDO! */
	TextOperation op;
	op.type = TextOperation::TYPE_REMOVE;
	op.from_line = p_from_line;
	op.from_column = p_from_column;
	op.to_line = p_to_line;
	op.to_column = p_to_column;
	op.text = text;
	op.version = ++version;
	op.chain_forward = false;
	op.chain_backward = false;

	// See if it should just be set as current op.
	if (current_op.type != op.type) {
		op.prev_version = get_version();
		_push_current_op();
		current_op = op;
		return; // Set as current op, return.
	}
	// See if it can be merged.
	if (current_op.from_line == p_to_line && current_op.from_column == p_to_column) {
		// Backspace or similar.
		current_op.text = text + current_op.text;
		current_op.from_line = p_from_line;
		current_op.from_column = p_from_column;
		return; // Update current op.
	}

	op.prev_version = get_version();
	_push_current_op();
	current_op = op;
}

void TextEdit::_insert_text_at_cursor(const String &p_text) {
	int new_column, new_line;
	_insert_text(cursor.line, cursor.column, p_text, &new_line, &new_column);
	_update_scrollbars();
	cursor_set_line(new_line, false);
	cursor_set_column(new_column);

	update();
}

void TextEdit::_line_edited_from(int p_line) {
	int cache_size = color_region_cache.size();
	for (int i = p_line; i < cache_size; i++) {
		color_region_cache.erase(i);
	}

	if (syntax_highlighting_cache.size() > 0) {
		cache_size = syntax_highlighting_cache.back()->key();
		for (int i = p_line - 1; i <= cache_size; i++) {
			if (syntax_highlighting_cache.has(i)) {
				syntax_highlighting_cache.erase(i);
			}
		}
	}
}

int TextEdit::get_char_count() {
	int totalsize = 0;

	for (int i = 0; i < text.size(); i++) {
		if (i > 0) {
			totalsize++; // Include \n.
		}
		totalsize += text[i].length();
	}

	return totalsize; // Omit last \n.
}
Size2 TextEdit::get_minimum_size() const {
	return cache.style_normal->get_minimum_size();
}

int TextEdit::_get_control_height() const {
	int control_height = get_size().height;
	control_height -= cache.style_normal->get_minimum_size().height;
	if (h_scroll->is_visible_in_tree()) {
		control_height -= h_scroll->get_size().height;
	}
	return control_height;
}

void TextEdit::_generate_context_menu() {
	// Reorganize context menu.
	menu->clear();
	if (!readonly) {
		menu->add_item(RTR("Cut"), MENU_CUT, is_shortcut_keys_enabled() ? KEY_MASK_CMD | KEY_X : 0);
	}
	menu->add_item(RTR("Copy"), MENU_COPY, is_shortcut_keys_enabled() ? KEY_MASK_CMD | KEY_C : 0);
	if (!readonly) {
		menu->add_item(RTR("Paste"), MENU_PASTE, is_shortcut_keys_enabled() ? KEY_MASK_CMD | KEY_V : 0);
	}
	if (selecting_enabled || !readonly) {
		menu->add_separator();
	}
	if (selecting_enabled) {
		menu->add_item(RTR("Select All"), MENU_SELECT_ALL, is_shortcut_keys_enabled() ? KEY_MASK_CMD | KEY_A : 0);
	}
	if (!readonly) {
		menu->add_item(RTR("Clear"), MENU_CLEAR);
		menu->add_separator();
		menu->add_item(RTR("Undo"), MENU_UNDO, is_shortcut_keys_enabled() ? KEY_MASK_CMD | KEY_Z : 0);
		menu->add_item(RTR("Redo"), MENU_REDO, is_shortcut_keys_enabled() ? KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_Z : 0);
	}
}

int TextEdit::get_visible_rows() const {
	return _get_control_height() / get_row_height();
}

int TextEdit::_get_minimap_visible_rows() const {
	return _get_control_height() / (minimap_char_size.y + minimap_line_spacing);
}

int TextEdit::get_total_visible_rows() const {
	// Returns the total amount of rows we need in the editor.
	// This skips hidden lines and counts each wrapping of a line.
	if (!is_hiding_enabled() && !is_wrap_enabled()) {
		return text.size();
	}

	int total_rows = 0;
	for (int i = 0; i < text.size(); i++) {
		if (!text.is_hidden(i)) {
			total_rows++;
			total_rows += times_line_wraps(i);
		}
	}
	return total_rows;
}

void TextEdit::_update_wrap_at() {
	wrap_at = get_size().width - cache.style_normal->get_minimum_size().width - cache.line_number_w - cache.breakpoint_gutter_width - cache.fold_gutter_width - cache.info_gutter_width - cache.minimap_width - wrap_right_offset;
	update_cursor_wrap_offset();
	text.clear_wrap_cache();

	for (int i = 0; i < text.size(); i++) {
		// Update all values that wrap.
		if (!line_wraps(i)) {
			continue;
		}
		Vector<String> rows = get_wrap_rows_text(i);
		text.set_line_wrap_amount(i, rows.size() - 1);
	}
}

void TextEdit::adjust_viewport_to_cursor() {
	// Make sure cursor is visible on the screen.
	scrolling = false;
	minimap_clicked = false;

	int cur_line = cursor.line;
	int cur_wrap = get_cursor_wrap_index();

	int first_vis_line = get_first_visible_line();
	int first_vis_wrap = cursor.wrap_ofs;
	int last_vis_line = get_last_full_visible_line();
	int last_vis_wrap = get_last_full_visible_line_wrap_index();

	if (cur_line < first_vis_line || (cur_line == first_vis_line && cur_wrap < first_vis_wrap)) {
		// Cursor is above screen.
		set_line_as_first_visible(cur_line, cur_wrap);
	} else if (cur_line > last_vis_line || (cur_line == last_vis_line && cur_wrap > last_vis_wrap)) {
		// Cursor is below screen.
		set_line_as_last_visible(cur_line, cur_wrap);
	}

	int visible_width = get_size().width - cache.style_normal->get_minimum_size().width - cache.line_number_w - cache.breakpoint_gutter_width - cache.fold_gutter_width - cache.info_gutter_width - cache.minimap_width;
	if (v_scroll->is_visible_in_tree()) {
		visible_width -= v_scroll->get_combined_minimum_size().width;
	}
	visible_width -= 20; // Give it a little more space.

	if (!is_wrap_enabled()) {
		// Adjust x offset.
		int cursor_x = get_column_x_offset(cursor.column, text[cursor.line]);

		if (cursor_x > (cursor.x_ofs + visible_width)) {
			cursor.x_ofs = cursor_x - visible_width + 1;
		}

		if (cursor_x < cursor.x_ofs) {
			cursor.x_ofs = cursor_x;
		}
	} else {
		cursor.x_ofs = 0;
	}
	h_scroll->set_value(cursor.x_ofs);

	update();
}

void TextEdit::center_viewport_to_cursor() {
	// Move viewport so the cursor is in the center of the screen.
	scrolling = false;
	minimap_clicked = false;

	if (is_line_hidden(cursor.line)) {
		unfold_line(cursor.line);
	}

	set_line_as_center_visible(cursor.line, get_cursor_wrap_index());
	int visible_width = get_size().width - cache.style_normal->get_minimum_size().width - cache.line_number_w - cache.breakpoint_gutter_width - cache.fold_gutter_width - cache.info_gutter_width - cache.minimap_width;
	if (v_scroll->is_visible_in_tree()) {
		visible_width -= v_scroll->get_combined_minimum_size().width;
	}
	visible_width -= 20; // Give it a little more space.

	if (is_wrap_enabled()) {
		// Center x offset.
		int cursor_x = get_column_x_offset_for_line(cursor.column, cursor.line);

		if (cursor_x > (cursor.x_ofs + visible_width)) {
			cursor.x_ofs = cursor_x - visible_width + 1;
		}

		if (cursor_x < cursor.x_ofs) {
			cursor.x_ofs = cursor_x;
		}
	} else {
		cursor.x_ofs = 0;
	}
	h_scroll->set_value(cursor.x_ofs);

	update();
}

void TextEdit::update_cursor_wrap_offset() {
	int first_vis_line = get_first_visible_line();
	if (line_wraps(first_vis_line)) {
		cursor.wrap_ofs = MIN(cursor.wrap_ofs, times_line_wraps(first_vis_line));
	} else {
		cursor.wrap_ofs = 0;
	}
	set_line_as_first_visible(cursor.line_ofs, cursor.wrap_ofs);
}

bool TextEdit::line_wraps(int line) const {
	ERR_FAIL_INDEX_V(line, text.size(), 0);
	if (!is_wrap_enabled()) {
		return false;
	}
	return text.get_line_width(line) > wrap_at;
}

int TextEdit::times_line_wraps(int line) const {
	ERR_FAIL_INDEX_V(line, text.size(), 0);
	if (!line_wraps(line)) {
		return 0;
	}

	int wrap_amount = text.get_line_wrap_amount(line);
	if (wrap_amount == -1) {
		// Update the value.
		Vector<String> rows = get_wrap_rows_text(line);
		wrap_amount = rows.size() - 1;
		text.set_line_wrap_amount(line, wrap_amount);
	}

	return wrap_amount;
}

Vector<String> TextEdit::get_wrap_rows_text(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), Vector<String>());

	Vector<String> lines;
	if (!line_wraps(p_line)) {
		lines.push_back(text[p_line]);
		return lines;
	}

	int px = 0;
	int col = 0;
	String line_text = text[p_line];
	String wrap_substring = "";

	int word_px = 0;
	String word_str = "";
	int cur_wrap_index = 0;

	int tab_offset_px = get_indent_level(p_line) * cache.font->get_char_size(' ').width;
	if (tab_offset_px >= wrap_at) {
		tab_offset_px = 0;
	}

	while (col < line_text.length()) {
		CharType c = line_text[col];
		int w = text.get_char_width(c, line_text[col + 1], px + word_px);

		int indent_ofs = (cur_wrap_index != 0 ? tab_offset_px : 0);

		if (indent_ofs + word_px + w > wrap_at) {
			// Not enough space to add this char; start next line.
			wrap_substring += word_str;
			lines.push_back(wrap_substring);
			cur_wrap_index++;
			wrap_substring = "";
			px = 0;

			word_str = "";
			word_str += c;
			word_px = w;
		} else {
			word_str += c;
			word_px += w;
			if (c == ' ') {
				// End of a word; add this word to the substring.
				wrap_substring += word_str;
				px += word_px;
				word_str = "";
				word_px = 0;
			}

			if (indent_ofs + px + word_px > wrap_at) {
				// This word will be moved to the next line.
				lines.push_back(wrap_substring);
				// Reset for next wrap.
				cur_wrap_index++;
				wrap_substring = "";
				px = 0;
			}
		}
		col++;
	}
	// Line ends before hit wrap_at; add this word to the substring.
	wrap_substring += word_str;
	lines.push_back(wrap_substring);

	// Update cache.
	text.set_line_wrap_amount(p_line, lines.size() - 1);

	return lines;
}

int TextEdit::get_cursor_wrap_index() const {
	return get_line_wrap_index_at_col(cursor.line, cursor.column);
}

int TextEdit::get_line_wrap_index_at_col(int p_line, int p_column) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), 0);

	if (!line_wraps(p_line)) {
		return 0;
	}

	// Loop through wraps in the line text until we get to the column.
	int wrap_index = 0;
	int col = 0;
	Vector<String> rows = get_wrap_rows_text(p_line);
	for (int i = 0; i < rows.size(); i++) {
		wrap_index = i;
		String s = rows[wrap_index];
		col += s.length();
		if (col > p_column) {
			break;
		}
	}
	return wrap_index;
}

void TextEdit::cursor_set_column(int p_col, bool p_adjust_viewport) {
	if (p_col < 0) {
		p_col = 0;
	}

	cursor.column = p_col;
	if (cursor.column > get_line(cursor.line).length()) {
		cursor.column = get_line(cursor.line).length();
	}

	cursor.last_fit_x = get_column_x_offset_for_line(cursor.column, cursor.line);

	if (p_adjust_viewport) {
		adjust_viewport_to_cursor();
	}

	if (!cursor_changed_dirty) {
		if (is_inside_tree()) {
			MessageQueue::get_singleton()->push_call(this, "_cursor_changed_emit");
		}
		cursor_changed_dirty = true;
	}
}

void TextEdit::cursor_set_line(int p_row, bool p_adjust_viewport, bool p_can_be_hidden, int p_wrap_index) {
	if (setting_row) {
		return;
	}

	setting_row = true;
	if (p_row < 0) {
		p_row = 0;
	}

	if (p_row >= text.size()) {
		p_row = text.size() - 1;
	}

	if (!p_can_be_hidden) {
		if (is_line_hidden(CLAMP(p_row, 0, text.size() - 1))) {
			int move_down = num_lines_from(p_row, 1) - 1;
			if (p_row + move_down <= text.size() - 1 && !is_line_hidden(p_row + move_down)) {
				p_row += move_down;
			} else {
				int move_up = num_lines_from(p_row, -1) - 1;
				if (p_row - move_up > 0 && !is_line_hidden(p_row - move_up)) {
					p_row -= move_up;
				} else {
					WARN_PRINT(("Cursor set to hidden line " + itos(p_row) + " and there are no nonhidden lines."));
				}
			}
		}
	}
	cursor.line = p_row;

	int n_col = get_char_pos_for_line(cursor.last_fit_x, p_row, p_wrap_index);
	if (n_col != 0 && is_wrap_enabled() && p_wrap_index < times_line_wraps(p_row)) {
		Vector<String> rows = get_wrap_rows_text(p_row);
		int row_end_col = 0;
		for (int i = 0; i < p_wrap_index + 1; i++) {
			row_end_col += rows[i].length();
		}
		if (n_col >= row_end_col) {
			n_col -= 1;
		}
	}
	cursor.column = n_col;

	if (p_adjust_viewport) {
		adjust_viewport_to_cursor();
	}

	setting_row = false;

	if (!cursor_changed_dirty) {
		if (is_inside_tree()) {
			MessageQueue::get_singleton()->push_call(this, "_cursor_changed_emit");
		}
		cursor_changed_dirty = true;
	}
}

int TextEdit::cursor_get_column() const {
	return cursor.column;
}

int TextEdit::cursor_get_line() const {
	return cursor.line;
}

bool TextEdit::cursor_get_blink_enabled() const {
	return caret_blink_enabled;
}

void TextEdit::cursor_set_blink_enabled(const bool p_enabled) {
	caret_blink_enabled = p_enabled;

	if (has_focus()) {
		if (p_enabled) {
			caret_blink_timer->start();
		} else {
			caret_blink_timer->stop();
		}
	}

	draw_caret = true;
}

float TextEdit::cursor_get_blink_speed() const {
	return caret_blink_timer->get_wait_time();
}

void TextEdit::cursor_set_blink_speed(const float p_speed) {
	ERR_FAIL_COND(p_speed <= 0);
	caret_blink_timer->set_wait_time(p_speed);
}

void TextEdit::cursor_set_block_mode(const bool p_enable) {
	block_caret = p_enable;
	update();
}

bool TextEdit::cursor_is_block_mode() const {
	return block_caret;
}

void TextEdit::set_right_click_moves_caret(bool p_enable) {
	right_click_moves_caret = p_enable;
}

bool TextEdit::is_right_click_moving_caret() const {
	return right_click_moves_caret;
}

void TextEdit::_v_scroll_input() {
	scrolling = false;
	minimap_clicked = false;
}

void TextEdit::_scroll_moved(double p_to_val) {
	if (updating_scrolls) {
		return;
	}

	if (h_scroll->is_visible_in_tree()) {
		cursor.x_ofs = h_scroll->get_value();
	}
	if (v_scroll->is_visible_in_tree()) {
		// Set line ofs and wrap ofs.
		int v_scroll_i = floor(get_v_scroll());
		int sc = 0;
		int n_line;
		for (n_line = 0; n_line < text.size(); n_line++) {
			if (!is_line_hidden(n_line)) {
				sc++;
				sc += times_line_wraps(n_line);
				if (sc > v_scroll_i) {
					break;
				}
			}
		}
		n_line = MIN(n_line, text.size() - 1);
		int line_wrap_amount = times_line_wraps(n_line);
		int wi = line_wrap_amount - (sc - v_scroll_i - 1);
		wi = CLAMP(wi, 0, line_wrap_amount);

		cursor.line_ofs = n_line;
		cursor.wrap_ofs = wi;
	}
	update();
}

int TextEdit::get_row_height() const {
	return cache.font->get_height() + cache.line_spacing;
}