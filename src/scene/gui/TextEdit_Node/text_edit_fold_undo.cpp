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

void TextEdit::set_line_as_marked(int p_line, bool p_marked) {
	ERR_FAIL_INDEX(p_line, text.size());
	text.set_marked(p_line, p_marked);
	update();
}

void TextEdit::set_line_as_safe(int p_line, bool p_safe) {
	ERR_FAIL_INDEX(p_line, text.size());
	text.set_safe(p_line, p_safe);
	update();
}

bool TextEdit::is_line_set_as_safe(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), false);
	return text.is_safe(p_line);
}

void TextEdit::set_executing_line(int p_line) {
	ERR_FAIL_INDEX(p_line, text.size());
	executing_line = p_line;
	update();
}

void TextEdit::clear_executing_line() {
	executing_line = -1;
	update();
}

bool TextEdit::is_line_set_as_bookmark(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), false);
	return text.is_bookmark(p_line);
}

void TextEdit::set_line_as_bookmark(int p_line, bool p_bookmark) {
	ERR_FAIL_INDEX(p_line, text.size());
	text.set_bookmark(p_line, p_bookmark);
	update();
}

void TextEdit::get_bookmarks(List<int> *p_bookmarks) const {
	for (int i = 0; i < text.size(); i++) {
		if (text.is_bookmark(i)) {
			p_bookmarks->push_back(i);
		}
	}
}

Array TextEdit::get_bookmarks_array() const {
	Array arr;
	for (int i = 0; i < text.size(); i++) {
		if (text.is_bookmark(i)) {
			arr.append(i);
		}
	}
	return arr;
}

bool TextEdit::is_line_set_as_breakpoint(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), false);
	return text.is_breakpoint(p_line);
}

void TextEdit::set_line_as_breakpoint(int p_line, bool p_breakpoint) {
	ERR_FAIL_INDEX(p_line, text.size());
	text.set_breakpoint(p_line, p_breakpoint);
	update();
}

void TextEdit::get_breakpoints(List<int> *p_breakpoints) const {
	for (int i = 0; i < text.size(); i++) {
		if (text.is_breakpoint(i)) {
			p_breakpoints->push_back(i);
		}
	}
}

Array TextEdit::get_breakpoints_array() const {
	Array arr;
	for (int i = 0; i < text.size(); i++) {
		if (text.is_breakpoint(i)) {
			arr.append(i);
		}
	}
	return arr;
}

void TextEdit::remove_breakpoints() {
	for (int i = 0; i < text.size(); i++) {
		if (text.is_breakpoint(i)) {
			/* Should "breakpoint_toggled" be fired when breakpoints are removed this way? */
			text.set_breakpoint(i, false);
		}
	}
}

void TextEdit::set_line_info_icon(int p_line, Ref<Texture> p_icon, String p_info) {
	ERR_FAIL_INDEX(p_line, text.size());
	text.set_info_icon(p_line, p_icon, p_info);
	update();
}

void TextEdit::clear_info_icons() {
	text.clear_info_icons();
	update();
}

void TextEdit::set_line_as_hidden(int p_line, bool p_hidden) {
	ERR_FAIL_INDEX(p_line, text.size());
	if (is_hiding_enabled() || !p_hidden) {
		text.set_hidden(p_line, p_hidden);
	}
	update();
}

bool TextEdit::is_line_hidden(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), false);
	return text.is_hidden(p_line);
}

void TextEdit::fold_all_lines() {
	for (int i = 0; i < text.size(); i++) {
		fold_line(i);
	}
	_update_scrollbars();
	update();
}

void TextEdit::unhide_all_lines() {
	for (int i = 0; i < text.size(); i++) {
		text.set_hidden(i, false);
	}
	_update_scrollbars();
	update();
}

int TextEdit::num_lines_from(int p_line_from, int visible_amount) const {
	// Returns the number of lines (hidden and unhidden) from p_line_from to (p_line_from + visible_amount of unhidden lines).
	ERR_FAIL_INDEX_V(p_line_from, text.size(), ABS(visible_amount));

	if (!is_hiding_enabled()) {
		return ABS(visible_amount);
	}

	int num_visible = 0;
	int num_total = 0;
	if (visible_amount >= 0) {
		for (int i = p_line_from; i < text.size(); i++) {
			num_total++;
			if (!is_line_hidden(i)) {
				num_visible++;
			}
			if (num_visible >= visible_amount) {
				break;
			}
		}
	} else {
		visible_amount = ABS(visible_amount);
		for (int i = p_line_from; i >= 0; i--) {
			num_total++;
			if (!is_line_hidden(i)) {
				num_visible++;
			}
			if (num_visible >= visible_amount) {
				break;
			}
		}
	}
	return num_total;
}

int TextEdit::num_lines_from_rows(int p_line_from, int p_wrap_index_from, int visible_amount, int &wrap_index) const {
	// Returns the number of lines (hidden and unhidden) from (p_line_from + p_wrap_index_from) row to (p_line_from + visible_amount of unhidden and wrapped rows).
	// Wrap index is set to the wrap index of the last line.
	wrap_index = 0;
	ERR_FAIL_INDEX_V(p_line_from, text.size(), ABS(visible_amount));

	if (!is_hiding_enabled() && !is_wrap_enabled()) {
		return ABS(visible_amount);
	}

	int num_visible = 0;
	int num_total = 0;
	if (visible_amount == 0) {
		num_total = 0;
		wrap_index = 0;
	} else if (visible_amount > 0) {
		int i;
		num_visible -= p_wrap_index_from;
		for (i = p_line_from; i < text.size(); i++) {
			num_total++;
			if (!is_line_hidden(i)) {
				num_visible++;
				num_visible += times_line_wraps(i);
			}
			if (num_visible >= visible_amount) {
				break;
			}
		}
		wrap_index = times_line_wraps(MIN(i, text.size() - 1)) - (num_visible - visible_amount);
	} else {
		visible_amount = ABS(visible_amount);
		int i;
		num_visible -= times_line_wraps(p_line_from) - p_wrap_index_from;
		for (i = p_line_from; i >= 0; i--) {
			num_total++;
			if (!is_line_hidden(i)) {
				num_visible++;
				num_visible += times_line_wraps(i);
			}
			if (num_visible >= visible_amount) {
				break;
			}
		}
		wrap_index = (num_visible - visible_amount);
	}
	wrap_index = MAX(wrap_index, 0);
	return num_total;
}

int TextEdit::get_last_unhidden_line() const {
	// Returns the last line in the text that is not hidden.
	if (!is_hiding_enabled()) {
		return text.size() - 1;
	}

	int last_line;
	for (last_line = text.size() - 1; last_line > 0; last_line--) {
		if (!is_line_hidden(last_line)) {
			break;
		}
	}
	return last_line;
}

int TextEdit::get_indent_level(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), 0);

	// Counts number of tabs and spaces before line starts.
	int tab_count = 0;
	int whitespace_count = 0;
	int line_length = text[p_line].size();
	for (int i = 0; i < line_length - 1; i++) {
		if (text[p_line][i] == '\t') {
			tab_count++;
		} else if (text[p_line][i] == ' ') {
			whitespace_count++;
		} else {
			break;
		}
	}
	return tab_count * indent_size + whitespace_count;
}

bool TextEdit::is_line_comment(int p_line) const {
	// Checks to see if this line is the start of a comment.
	ERR_FAIL_INDEX_V(p_line, text.size(), false);

	const Map<int, Text::ColorRegionInfo> &cri_map = text.get_color_region_info(p_line);

	int line_length = text[p_line].size();
	for (int i = 0; i < line_length - 1; i++) {
		if (_is_symbol(text[p_line][i]) && cri_map.has(i)) {
			const Text::ColorRegionInfo &cri = cri_map[i];
			return color_regions[cri.region].begin_key == "#" || color_regions[cri.region].begin_key == "//";
		} else if (_is_whitespace(text[p_line][i])) {
			continue;
		} else {
			break;
		}
	}
	return false;
}

bool TextEdit::can_fold(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), false);
	if (!is_hiding_enabled()) {
		return false;
	}
	if (p_line + 1 >= text.size()) {
		return false;
	}
	if (text[p_line].strip_edges().size() == 0) {
		return false;
	}
	if (is_folded(p_line)) {
		return false;
	}
	if (is_line_hidden(p_line)) {
		return false;
	}
	if (is_line_comment(p_line)) {
		return false;
	}

	int start_indent = get_indent_level(p_line);

	for (int i = p_line + 1; i < text.size(); i++) {
		if (text[i].strip_edges().size() == 0) {
			continue;
		}
		int next_indent = get_indent_level(i);
		if (is_line_comment(i)) {
			continue;
		} else if (next_indent > start_indent) {
			return true;
		} else {
			return false;
		}
	}

	return false;
}

bool TextEdit::is_folded(int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), false);
	if (p_line + 1 >= text.size()) {
		return false;
	}
	return !is_line_hidden(p_line) && is_line_hidden(p_line + 1);
}

Vector<int> TextEdit::get_folded_lines() const {
	Vector<int> folded_lines;

	for (int i = 0; i < text.size(); i++) {
		if (is_folded(i)) {
			folded_lines.push_back(i);
		}
	}
	return folded_lines;
}
void TextEdit::fold_line(int p_line) {
	ERR_FAIL_INDEX(p_line, text.size());
	if (!is_hiding_enabled()) {
		return;
	}
	if (!can_fold(p_line)) {
		return;
	}

	// Hide lines below this one.
	int start_indent = get_indent_level(p_line);
	int last_line = start_indent;
	for (int i = p_line + 1; i < text.size(); i++) {
		if (text[i].strip_edges().size() != 0) {
			if (is_line_comment(i) && get_indent_level(i) <= start_indent) {
				// Checked indent to make sure indented comments that finish a code block are folded.
				continue;
			} else if (get_indent_level(i) > start_indent) {
				last_line = i;
			} else {
				break;
			}
		}
	}
	for (int i = p_line + 1; i <= last_line; i++) {
		set_line_as_hidden(i, true);
	}

	// Fix selection.
	if (is_selection_active()) {
		if (is_line_hidden(selection.from_line) && is_line_hidden(selection.to_line)) {
			deselect();
		} else if (is_line_hidden(selection.from_line)) {
			select(p_line, 9999, selection.to_line, selection.to_column);
		} else if (is_line_hidden(selection.to_line)) {
			select(selection.from_line, selection.from_column, p_line, 9999);
		}
	}

	// Reset cursor.
	if (is_line_hidden(cursor.line)) {
		cursor_set_line(p_line, false, false);
		cursor_set_column(get_line(p_line).length(), false);
	}
	_update_scrollbars();
	update();
}

void TextEdit::unfold_line(int p_line) {
	ERR_FAIL_INDEX(p_line, text.size());

	if (!is_folded(p_line) && !is_line_hidden(p_line)) {
		return;
	}
	int fold_start;
	for (fold_start = p_line; fold_start > 0; fold_start--) {
		if (is_folded(fold_start)) {
			break;
		}
	}
	fold_start = is_folded(fold_start) ? fold_start : p_line;

	for (int i = fold_start + 1; i < text.size(); i++) {
		if (is_line_hidden(i)) {
			set_line_as_hidden(i, false);
		} else {
			break;
		}
	}
	_update_scrollbars();
	update();
}

void TextEdit::toggle_fold_line(int p_line) {
	ERR_FAIL_INDEX(p_line, text.size());

	if (!is_folded(p_line)) {
		fold_line(p_line);
	} else {
		unfold_line(p_line);
	}
}

int TextEdit::get_line_count() const {
	return text.size();
}

void TextEdit::_do_text_op(const TextOperation &p_op, bool p_reverse) {
	ERR_FAIL_COND(p_op.type == TextOperation::TYPE_NONE);

	bool insert = p_op.type == TextOperation::TYPE_INSERT;
	if (p_reverse) {
		insert = !insert;
	}

	if (insert) {
		int check_line;
		int check_column;
		_base_insert_text(p_op.from_line, p_op.from_column, p_op.text, check_line, check_column);
		ERR_FAIL_COND(check_line != p_op.to_line); // BUG.
		ERR_FAIL_COND(check_column != p_op.to_column); // BUG.
	} else {
		_base_remove_text(p_op.from_line, p_op.from_column, p_op.to_line, p_op.to_column);
	}
}

void TextEdit::_clear_redo() {
	if (undo_stack_pos == nullptr) {
		return; // Nothing to clear.
	}

	_push_current_op();

	while (undo_stack_pos) {
		List<TextOperation>::Element *elem = undo_stack_pos;
		undo_stack_pos = undo_stack_pos->next();
		undo_stack.erase(elem);
	}
}

bool TextEdit::has_undo() const {
	if (undo_stack_pos == nullptr) {
		int pending = current_op.type == TextOperation::TYPE_NONE ? 0 : 1;
		return undo_stack.size() + pending > 0;
	}
	return undo_stack_pos != undo_stack.front();
}

bool TextEdit::has_redo() const {
	return undo_stack_pos != nullptr;
}

void TextEdit::undo() {
	_push_current_op();

	if (undo_stack_pos == nullptr) {
		if (!undo_stack.size()) {
			return; // Nothing to undo.
		}

		undo_stack_pos = undo_stack.back();

	} else if (undo_stack_pos == undo_stack.front()) {
		return; // At the bottom of the undo stack.
	} else {
		undo_stack_pos = undo_stack_pos->prev();
	}

	deselect();

	TextOperation op = undo_stack_pos->get();
	_do_text_op(op, true);

	current_op.version = op.prev_version;
	if (undo_stack_pos->get().chain_backward) {
		while (true) {
			ERR_BREAK(!undo_stack_pos->prev());
			undo_stack_pos = undo_stack_pos->prev();
			op = undo_stack_pos->get();
			_do_text_op(op, true);
			current_op.version = op.prev_version;
			if (undo_stack_pos->get().chain_forward) {
				break;
			}
		}
	}

	if (op.type != TextOperation::TYPE_INSERT && (op.from_line != op.to_line || op.to_column != op.from_column + 1)) {
		select(op.from_line, op.from_column, op.to_line, op.to_column);
	}

	_update_scrollbars();
	if (undo_stack_pos->get().type == TextOperation::TYPE_REMOVE) {
		cursor_set_line(undo_stack_pos->get().to_line, false);
		cursor_set_column(undo_stack_pos->get().to_column);
		_cancel_code_hint();
	} else {
		cursor_set_line(undo_stack_pos->get().from_line, false);
		cursor_set_column(undo_stack_pos->get().from_column);
	}
	update();
}
void TextEdit::redo() {
	_push_current_op();

	if (undo_stack_pos == nullptr) {
		return; // Nothing to do.
	}

	deselect();

	TextOperation op = undo_stack_pos->get();
	_do_text_op(op, false);
	current_op.version = op.version;
	if (undo_stack_pos->get().chain_forward) {
		while (true) {
			ERR_BREAK(!undo_stack_pos->next());
			undo_stack_pos = undo_stack_pos->next();
			op = undo_stack_pos->get();
			_do_text_op(op, false);
			current_op.version = op.version;
			if (undo_stack_pos->get().chain_backward) {
				break;
			}
		}
	}

	_update_scrollbars();
	cursor_set_line(undo_stack_pos->get().to_line, false);
	cursor_set_column(undo_stack_pos->get().to_column);
	undo_stack_pos = undo_stack_pos->next();
	update();
}

void TextEdit::clear_undo_history() {
	saved_version = 0;
	current_op.type = TextOperation::TYPE_NONE;
	undo_stack_pos = nullptr;
	undo_stack.clear();
}

void TextEdit::begin_complex_operation() {
	_push_current_op();
	next_operation_is_complex = true;
}

void TextEdit::end_complex_operation() {
	_push_current_op();
	ERR_FAIL_COND(undo_stack.size() == 0);

	if (undo_stack.back()->get().chain_forward) {
		undo_stack.back()->get().chain_forward = false;
		return;
	}

	undo_stack.back()->get().chain_backward = true;
}

void TextEdit::_push_current_op() {
	if (current_op.type == TextOperation::TYPE_NONE) {
		return; // Nothing to do.
	}

	if (next_operation_is_complex) {
		current_op.chain_forward = true;
		next_operation_is_complex = false;
	}

	undo_stack.push_back(current_op);
	current_op.type = TextOperation::TYPE_NONE;
	current_op.text = "";
	current_op.chain_forward = false;

	if (undo_stack.size() > undo_stack_max_size) {
		undo_stack.pop_front();
	}
}

void TextEdit::set_indent_using_spaces(const bool p_use_spaces) {
	indent_using_spaces = p_use_spaces;
}

bool TextEdit::is_indent_using_spaces() const {
	return indent_using_spaces;
}

void TextEdit::set_indent_size(const int p_size) {
	ERR_FAIL_COND_MSG(p_size <= 0, "Indend size must be greater than 0.");
	indent_size = p_size;
	text.set_indent_size(p_size);

	space_indent = "";
	for (int i = 0; i < p_size; i++) {
		space_indent += " ";
	}

	update();
}

int TextEdit::get_indent_size() {
	return indent_size;
}

void TextEdit::set_draw_tabs(bool p_draw) {
	draw_tabs = p_draw;
	update();
}

bool TextEdit::is_drawing_tabs() const {
	return draw_tabs;
}

void TextEdit::set_draw_spaces(bool p_draw) {
	draw_spaces = p_draw;
}

bool TextEdit::is_drawing_spaces() const {
	return draw_spaces;
}

void TextEdit::set_override_selected_font_color(bool p_override_selected_font_color) {
	override_selected_font_color = p_override_selected_font_color;
}

bool TextEdit::is_overriding_selected_font_color() const {
	return override_selected_font_color;
}

void TextEdit::set_insert_mode(bool p_enabled) {
	insert_mode = p_enabled;
	update();
}

bool TextEdit::is_insert_mode() const {
	return insert_mode;
}

bool TextEdit::is_insert_text_operation() {
	return (current_op.type == TextOperation::TYPE_INSERT);
}

uint32_t TextEdit::get_version() const {
	return current_op.version;
}

uint32_t TextEdit::get_saved_version() const {
	return saved_version;
}

void TextEdit::tag_saved_version() {
	saved_version = get_version();
}

double TextEdit::get_scroll_pos_for_line(int p_line, int p_wrap_index) const {
	if (!is_wrap_enabled() && !is_hiding_enabled()) {
		return p_line;
	}

	// Count the number of visible lines up to this line.
	double new_line_scroll_pos = 0;
	int to = CLAMP(p_line, 0, text.size() - 1);
	for (int i = 0; i < to; i++) {
		if (!text.is_hidden(i)) {
			new_line_scroll_pos++;
			new_line_scroll_pos += times_line_wraps(i);
		}
	}
	new_line_scroll_pos += p_wrap_index;
	return new_line_scroll_pos;
}

void TextEdit::set_line_as_first_visible(int p_line, int p_wrap_index) {
	set_v_scroll(get_scroll_pos_for_line(p_line, p_wrap_index));
}

void TextEdit::set_line_as_center_visible(int p_line, int p_wrap_index) {
	int visible_rows = get_visible_rows();
	int wi;
	int first_line = p_line - num_lines_from_rows(p_line, p_wrap_index, -visible_rows / 2, wi) + 1;

	set_v_scroll(get_scroll_pos_for_line(first_line, wi));
}

void TextEdit::set_line_as_last_visible(int p_line, int p_wrap_index) {
	int wi;
	int first_line = p_line - num_lines_from_rows(p_line, p_wrap_index, -get_visible_rows() - 1, wi) + 1;

	set_v_scroll(get_scroll_pos_for_line(first_line, wi) + get_visible_rows_offset());
}

int TextEdit::get_first_visible_line() const {
	return CLAMP(cursor.line_ofs, 0, text.size() - 1);
}

int TextEdit::get_last_full_visible_line() const {
	int first_vis_line = get_first_visible_line();
	int last_vis_line = 0;
	int wi;
	last_vis_line = first_vis_line + num_lines_from_rows(first_vis_line, cursor.wrap_ofs, get_visible_rows(), wi) - 1;
	last_vis_line = CLAMP(last_vis_line, 0, text.size() - 1);
	return last_vis_line;
}

int TextEdit::get_last_full_visible_line_wrap_index() const {
	int first_vis_line = get_first_visible_line();
	int wi;
	num_lines_from_rows(first_vis_line, cursor.wrap_ofs, get_visible_rows(), wi);
	return wi;
}

double TextEdit::get_visible_rows_offset() const {
	double total = _get_control_height();
	total /= (double)get_row_height();
	total = total - floor(total);
	total = -CLAMP(total, 0.001, 1) + 1;
	return total;
}

double TextEdit::get_v_scroll_offset() const {
	double val = get_v_scroll() - floor(get_v_scroll());
	return CLAMP(val, 0, 1);
}

double TextEdit::get_v_scroll() const {
	return v_scroll->get_value();
}

void TextEdit::set_v_scroll(double p_scroll) {
	v_scroll->set_value(p_scroll);
	int max_v_scroll = v_scroll->get_max() - v_scroll->get_page();
	if (p_scroll >= max_v_scroll - 1.0) {
		_scroll_moved(v_scroll->get_value());
	}
}

int TextEdit::get_h_scroll() const {
	return h_scroll->get_value();
}

void TextEdit::set_h_scroll(int p_scroll) {
	if (p_scroll < 0) {
		p_scroll = 0;
	}
	h_scroll->set_value(p_scroll);
}

void TextEdit::set_smooth_scroll_enabled(bool p_enable) {
	v_scroll->set_smooth_scroll_enabled(p_enable);
	smooth_scroll_enabled = p_enable;
}

bool TextEdit::is_smooth_scroll_enabled() const {
	return smooth_scroll_enabled;
}

void TextEdit::set_v_scroll_speed(float p_speed) {
	v_scroll_speed = p_speed;
}

float TextEdit::get_v_scroll_speed() const {
	return v_scroll_speed;
}

