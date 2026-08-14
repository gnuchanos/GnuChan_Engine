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

/* Line and character position. */
Point2 TextEdit::get_pos_at_line_column(int p_line, int p_column) const {
	Rect2i rect = get_rect_at_line_column(p_line, p_column);
	return rect.position + Vector2i(0, get_line_height());
}

Rect2 TextEdit::get_rect_at_line_column(int p_line, int p_column) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), Rect2i(-1, -1, 0, 0));
	ERR_FAIL_COND_V(p_column < 0, Rect2i(-1, -1, 0, 0));
	ERR_FAIL_COND_V(p_column > text[p_line].length(), Rect2i(-1, -1, 0, 0));

	if (line_drawing_cache.size() == 0 || !line_drawing_cache.has(p_line)) {
		// Line is not in the cache, which means it's outside of the viewing area.
		return Rect2i(-1, -1, 0, 0);
	}
	LineDrawingCache cache_entry = line_drawing_cache[p_line];

	int wrap_index = get_line_wrap_index_at_col(p_line, p_column);
	if (wrap_index >= cache_entry.first_visible_char.size()) {
		// Line seems to be wrapped beyond the viewable area.
		return Rect2i(-1, -1, 0, 0);
	}

	int first_visible_char = cache_entry.first_visible_char[wrap_index];
	int last_visible_char = cache_entry.last_visible_char[wrap_index];
	if (p_column < first_visible_char || p_column > (last_visible_char + 1)) {
		// Character is outside of the viewing area, no point calculating its position.
		return Rect2i(-1, -1, 0, 0);
	}

	Point2i pos, size;
	pos.y = cache_entry.y_offset + get_line_height() * wrap_index;
	pos.x = get_total_gutter_width() + cache.style_normal->get_margin(MARGIN_LEFT) - get_h_scroll();

	int start_x = get_column_x_offset_for_line(p_column, p_line);
	pos.x += start_x;

	String line = text[p_line];
	size.x = cache.font->get_char_size(line[p_column]).width;
	size.y = get_line_height();

	return Rect2i(pos, size);
}

Point2 TextEdit::get_line_column_at_pos(const Point2 &p_pos) const {
	int row, col;
	_get_mouse_pos(p_pos, row, col);

	return Point2i(col, row);
}

int TextEdit::get_char_pos_for_line(int p_px, int p_line, int p_wrap_index) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), 0);

	if (line_wraps(p_line)) {
		int line_wrap_amount = times_line_wraps(p_line);
		int wrap_offset_px = get_indent_level(p_line) * cache.font->get_char_size(' ').width;
		if (wrap_offset_px >= wrap_at) {
			wrap_offset_px = 0;
		}
		if (p_wrap_index > line_wrap_amount) {
			p_wrap_index = line_wrap_amount;
		}
		if (p_wrap_index > 0) {
			p_px -= wrap_offset_px;
		} else {
			p_wrap_index = 0;
		}
		Vector<String> rows = get_wrap_rows_text(p_line);
		int c_pos = get_char_pos_for(p_px, rows[p_wrap_index]);
		for (int i = 0; i < p_wrap_index; i++) {
			String s = rows[i];
			c_pos += s.length();
		}

		return c_pos;
	} else {
		return get_char_pos_for(p_px, text[p_line]);
	}
}

int TextEdit::get_column_x_offset_for_line(int p_char, int p_line) const {
	ERR_FAIL_INDEX_V(p_line, text.size(), 0);

	if (line_wraps(p_line)) {
		int n_char = p_char;
		int col = 0;
		Vector<String> rows = get_wrap_rows_text(p_line);
		int wrap_index = 0;
		for (int i = 0; i < rows.size(); i++) {
			wrap_index = i;
			String s = rows[wrap_index];
			col += s.length();
			if (col > p_char) {
				break;
			}
			n_char -= s.length();
		}
		int px = get_column_x_offset(n_char, rows[wrap_index]);

		int wrap_offset_px = get_indent_level(p_line) * cache.font->get_char_size(' ').width;
		if (wrap_offset_px >= wrap_at) {
			wrap_offset_px = 0;
		}
		if (wrap_index != 0) {
			px += wrap_offset_px;
		}

		return px;
	} else {
		return get_column_x_offset(p_char, text[p_line]);
	}
}

int TextEdit::get_char_pos_for(int p_px, String p_str) const {
	int px = 0;
	int c = 0;

	while (c < p_str.length()) {
		int w = text.get_char_width(p_str[c], p_str[c + 1], px);

		if (p_px < (px + w / 2)) {
			break;
		}
		px += w;
		c++;
	}

	return c;
}

int TextEdit::get_column_x_offset(int p_char, String p_str) const {
	int px = 0;

	for (int i = 0; i < p_char; i++) {
		if (i >= p_str.length()) {
			break;
		}

		px += text.get_char_width(p_str[i], p_str[i + 1], px);
	}

	return px;
}

void TextEdit::insert_text_at_cursor(const String &p_text) {
	if (selection.active) {
		cursor_set_line(selection.from_line, false);
		cursor_set_column(selection.from_column);

		_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
		selection.active = false;
		selection.selecting_mode = Selection::MODE_NONE;
	}

	_insert_text_at_cursor(p_text);
	update();
}

Variant TextEdit::get_drag_data(const Point2 &p_point) {
	if (selection.active && selection.drag_attempt) {
		String t = get_selection_text();
		Label *l = memnew(Label);
		l->set_text(t);
		set_drag_preview(l);
		return t;
	}

	return Variant();
}

bool TextEdit::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	bool drop_override = Control::can_drop_data(p_point, p_data); // In case user wants to drop custom data.
	if (drop_override) {
		return drop_override;
	}

	return !readonly && p_data.get_type() == Variant::STRING;
}

void TextEdit::drop_data(const Point2 &p_point, const Variant &p_data) {
	Control::drop_data(p_point, p_data);

	if (p_data.get_type() == Variant::STRING && !readonly) {
		Point2 mp = get_local_mouse_position();
		int caret_row_tmp, caret_column_tmp;
		_get_mouse_pos(Point2i(mp.x, mp.y), caret_row_tmp, caret_column_tmp);
		if (selection.drag_attempt) {
			selection.drag_attempt = false;
			if (!is_mouse_over_selection(!Input::get_singleton()->is_key_pressed(KEY_CONTROL))) {
				begin_complex_operation();
				if (!Input::get_singleton()->is_key_pressed(KEY_CONTROL)) {
					if (caret_row_tmp > selection.to_line) {
						caret_row_tmp = caret_row_tmp - (selection.to_line - selection.from_line);
					} else if (caret_row_tmp == selection.to_line && caret_column_tmp >= selection.to_column) {
						caret_column_tmp = caret_column_tmp - (selection.to_column - selection.from_column);
					}

					_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
					cursor_set_line(selection.from_line, false);
					cursor_set_column(selection.from_column);
					selection.active = false;
					selection.selecting_mode = Selection::MODE_NONE;
				} else {
					deselect();
				}

				cursor_set_line(caret_row_tmp, true, false);
				cursor_set_column(caret_column_tmp);
				insert_text_at_cursor(p_data);
				end_complex_operation();
			}
		} else if (is_mouse_over_selection()) {
			begin_complex_operation();
			caret_row_tmp = selection.from_line;
			caret_column_tmp = selection.from_column;

			_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
			cursor_set_line(selection.from_line, false);
			cursor_set_column(selection.from_column);
			selection.active = false;
			selection.selecting_mode = Selection::MODE_NONE;

			cursor_set_line(caret_row_tmp, true, false);
			cursor_set_column(caret_column_tmp);
			insert_text_at_cursor(p_data);
			end_complex_operation();
			grab_focus();
		} else {
			deselect();
			cursor_set_line(caret_row_tmp, true, false);
			cursor_set_column(caret_column_tmp);
			insert_text_at_cursor(p_data);
			grab_focus();
		}

		if (caret_row_tmp != cursor.line || caret_column_tmp != cursor.column) {
			select(caret_row_tmp, caret_column_tmp, cursor.line, cursor.column);
		}
	}
}

bool TextEdit::is_mouse_over_selection(bool p_edges) const {
	if (!selection.active) {
		return false;
	}
	Point2 mp = get_local_mouse_position();
	int row, col;
	_get_mouse_pos(Point2i(mp.x, mp.y), row, col);
	if (p_edges) {
		if ((row == selection.from_line && col == selection.from_column) || (row == selection.to_line && col == selection.to_column)) {
			return true;
		}
	}
	return (row >= selection.from_line && row <= selection.to_line && (row > selection.from_line || col > selection.from_column) && (row < selection.to_line || col < selection.to_column));
}

Control::CursorShape TextEdit::get_cursor_shape(const Point2 &p_pos) const {
	if (highlighted_word != String()) {
		return CURSOR_POINTING_HAND;
	}

	if ((completion_active && completion_rect.has_point(p_pos)) || (is_readonly() && (!is_selecting_enabled() || text.size() == 0))) {
		return CURSOR_ARROW;
	}

	int gutter = cache.style_normal->get_margin(MARGIN_LEFT) + cache.line_number_w + cache.breakpoint_gutter_width + cache.fold_gutter_width + cache.info_gutter_width;
	if (p_pos.x < gutter) {
		int row, col;
		_get_mouse_pos(p_pos, row, col);
		int left_margin = cache.style_normal->get_margin(MARGIN_LEFT);

		// Breakpoint icon.
		if (draw_breakpoint_gutter && p_pos.x > left_margin - 6 && p_pos.x <= left_margin + cache.breakpoint_gutter_width - 3) {
			return CURSOR_POINTING_HAND;
		}

		// Info icons.
		int gutter_left = left_margin + cache.breakpoint_gutter_width + cache.info_gutter_width;
		if (draw_info_gutter && p_pos.x > left_margin + cache.breakpoint_gutter_width - 6 && p_pos.x <= gutter_left - 3) {
			if (text.has_info_icon(row)) {
				return CURSOR_POINTING_HAND;
			}
			return CURSOR_ARROW;
		}

		// Fold icon.
		if (draw_fold_gutter && p_pos.x > gutter_left + cache.line_number_w - 6 && p_pos.x <= gutter_left + cache.line_number_w + cache.fold_gutter_width - 3) {
			if (is_folded(row) || can_fold(row)) {
				return CURSOR_POINTING_HAND;
			} else {
				return CURSOR_ARROW;
			}
		}

		return CURSOR_ARROW;
	} else {
		int xmargin_end = get_size().width - cache.style_normal->get_margin(MARGIN_RIGHT);
		if (draw_minimap && p_pos.x > xmargin_end - minimap_width && p_pos.x <= xmargin_end) {
			return CURSOR_ARROW;
		}

		int row, col;
		_get_mouse_pos(p_pos, row, col);
		// EOL fold icon.
		if (is_folded(row)) {
			int line_width = text.get_line_width(row);
			line_width += cache.style_normal->get_margin(MARGIN_LEFT) + cache.line_number_w + cache.breakpoint_gutter_width + cache.fold_gutter_width + cache.info_gutter_width - cursor.x_ofs;
			if (p_pos.x > line_width - 3 && p_pos.x <= line_width + cache.folded_eol_icon->get_width() + 3) {
				return CURSOR_POINTING_HAND;
			}
		}
	}

	return get_default_cursor_shape();
}

void TextEdit::set_text(String p_text) {
	setting_text = true;
	if (!undo_enabled) {
		_clear();
		_insert_text_at_cursor(p_text);
	}

	if (undo_enabled) {
		cursor_set_line(0);
		cursor_set_column(0);

		begin_complex_operation();
		_remove_text(0, 0, MAX(0, get_line_count() - 1), MAX(get_line(MAX(get_line_count() - 1, 0)).size() - 1, 0));
		_insert_text_at_cursor(p_text);
		end_complex_operation();
		selection.active = false;
	}

	cursor_set_line(0);
	cursor_set_column(0);

	update();
	setting_text = false;
};

String TextEdit::get_text() {
	String longthing;
	int len = text.size();
	for (int i = 0; i < len; i++) {
		longthing += text[i];
		if (i != len - 1) {
			longthing += "\n";
		}
	}

	return longthing;
};

String TextEdit::get_text_for_lookup_completion() {
	int row, col;
	_get_mouse_pos(get_local_mouse_position(), row, col);

	String longthing;
	int len = text.size();
	for (int i = 0; i < len; i++) {
		if (i == row) {
			longthing += text[i].substr(0, col);
			longthing += String::chr(0xFFFF); // Not unicode, represents the cursor.
			longthing += text[i].substr(col, text[i].size());
		} else {
			longthing += text[i];
		}

		if (i != len - 1) {
			longthing += "\n";
		}
	}

	return longthing;
}

String TextEdit::get_text_for_completion() {
	String longthing;
	int len = text.size();
	for (int i = 0; i < len; i++) {
		if (i == cursor.line) {
			longthing += text[i].substr(0, cursor.column);
			longthing += String::chr(0xFFFF); // Not unicode, represents the cursor.
			longthing += text[i].substr(cursor.column, text[i].size());
		} else {
			longthing += text[i];
		}

		if (i != len - 1) {
			longthing += "\n";
		}
	}

	return longthing;
};

String TextEdit::get_line(int line) const {
	if (line < 0 || line >= text.size()) {
		return "";
	}

	return text[line];
};

void TextEdit::_clear() {
	clear_undo_history();
	text.clear();
	cursor.column = 0;
	cursor.line = 0;
	cursor.x_ofs = 0;
	cursor.line_ofs = 0;
	cursor.wrap_ofs = 0;
	cursor.last_fit_x = 0;
	selection.active = false;
}

void TextEdit::clear() {
	setting_text = true;
	_clear();
	setting_text = false;
};

void TextEdit::set_readonly(bool p_readonly) {
	if (readonly == p_readonly) {
		return;
	}

	readonly = p_readonly;
	_generate_context_menu();

	// Reorganize context menu.
	menu->clear();

	if (!readonly) {
		menu->add_item(RTR("Undo"), MENU_UNDO, KEY_MASK_CMD | KEY_Z);
		menu->add_item(RTR("Redo"), MENU_REDO, KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_Z);
	}

	if (!readonly) {
		menu->add_separator();
		menu->add_item(RTR("Cut"), MENU_CUT, KEY_MASK_CMD | KEY_X);
	}

	menu->add_item(RTR("Copy"), MENU_COPY, KEY_MASK_CMD | KEY_C);

	if (!readonly) {
		menu->add_item(RTR("Paste"), MENU_PASTE, KEY_MASK_CMD | KEY_V);
	}

	menu->add_separator();
	menu->add_item(RTR("Select All"), MENU_SELECT_ALL, KEY_MASK_CMD | KEY_A);

	if (!readonly) {
		menu->add_item(RTR("Clear"), MENU_CLEAR);
	}

	update();
}

bool TextEdit::is_readonly() const {
	return readonly;
}

void TextEdit::set_wrap_enabled(bool p_wrap_enabled) {
	wrap_enabled = p_wrap_enabled;
}

bool TextEdit::is_wrap_enabled() const {
	return wrap_enabled;
}

void TextEdit::_reset_caret_blink_timer() {
	if (caret_blink_enabled) {
		draw_caret = true;
		if (has_focus()) {
			caret_blink_timer->stop();
			caret_blink_timer->start();
			update();
		}
	}
}

void TextEdit::_toggle_draw_caret() {
	draw_caret = !draw_caret;
	if (is_visible_in_tree() && has_focus() && window_has_focus) {
		update();
	}
}

void TextEdit::_update_caches() {
	cache.style_normal = get_stylebox("normal");
	cache.style_focus = get_stylebox("focus");
	cache.style_readonly = get_stylebox("read_only");
	cache.completion_background_color = get_color("completion_background_color");
	cache.completion_selected_color = get_color("completion_selected_color");
	cache.completion_existing_color = get_color("completion_existing_color");
	cache.completion_font_color = get_color("completion_font_color");
	cache.font = get_font("font");
	cache.caret_color = get_color("caret_color");
	cache.caret_background_color = get_color("caret_background_color");
	cache.line_number_color = get_color("line_number_color");
	cache.safe_line_number_color = get_color("safe_line_number_color");
	cache.font_color = get_color("font_color");
	cache.font_color_selected = get_color("font_color_selected");
	cache.font_color_readonly = get_color("font_color_readonly");
	cache.keyword_color = get_color("keyword_color");
	cache.control_flow_keyword_color = get_color("control_flow_keyword_color");
	cache.function_color = get_color("function_color");
	cache.member_variable_color = get_color("member_variable_color");
	cache.number_color = get_color("number_color");
	cache.selection_color = get_color("selection_color");
	cache.mark_color = get_color("mark_color");
	cache.current_line_color = get_color("current_line_color");
	cache.line_length_guideline_color = get_color("line_length_guideline_color");
	cache.bookmark_color = get_color("bookmark_color");
	cache.breakpoint_color = get_color("breakpoint_color");
	cache.executing_line_color = get_color("executing_line_color");
	cache.code_folding_color = get_color("code_folding_color");
	cache.brace_mismatch_color = get_color("brace_mismatch_color");
	cache.word_highlighted_color = get_color("word_highlighted_color");
	cache.search_result_color = get_color("search_result_color");
	cache.search_result_border_color = get_color("search_result_border_color");
	cache.symbol_color = get_color("symbol_color");
	cache.background_color = get_color("background_color");
#ifdef TOOLS_ENABLED
	cache.line_spacing = get_constant("line_spacing") * EDSCALE;
#else
	cache.line_spacing = get_constant("line_spacing");
#endif
	cache.row_height = cache.font->get_height() + cache.line_spacing;
	cache.tab_icon = get_icon("tab");
	cache.space_icon = get_icon("space");
	cache.folded_icon = get_icon("folded");
	cache.can_fold_icon = get_icon("fold");
	cache.folded_eol_icon = get_icon("GuiEllipsis", "EditorIcons");
	cache.executing_icon = get_icon("TextEditorPlay", "EditorIcons");
	text.set_font(cache.font);

	if (syntax_highlighter) {
		syntax_highlighter->_update_cache();
	}
}

SyntaxHighlighter *TextEdit::_get_syntax_highlighting() {
	return syntax_highlighter;
}

void TextEdit::_set_syntax_highlighting(SyntaxHighlighter *p_syntax_highlighter) {
	syntax_highlighter = p_syntax_highlighter;
	if (syntax_highlighter) {
		syntax_highlighter->set_text_editor(this);
		syntax_highlighter->_update_cache();
	}
	syntax_highlighting_cache.clear();
	update();
}

int TextEdit::_is_line_in_region(int p_line) {
	// Do we have in cache?
	if (color_region_cache.has(p_line)) {
		return color_region_cache[p_line];
	}

	// If not find the closest line we have.
	int previous_line = p_line - 1;
	for (; previous_line > -1; previous_line--) {
		if (color_region_cache.has(previous_line)) {
			break;
		}
	}

	// Calculate up to line we need and update the cache along the way.
	int in_region = color_region_cache[previous_line];
	if (previous_line == -1) {
		in_region = -1;
	}
	for (int i = previous_line; i < p_line; i++) {
		const Map<int, Text::ColorRegionInfo> &cri_map = _get_line_color_region_info(i);
		for (const Map<int, Text::ColorRegionInfo>::Element *E = cri_map.front(); E; E = E->next()) {
			const Text::ColorRegionInfo &cri = E->get();
			if (in_region == -1) {
				if (!cri.end) {
					in_region = cri.region;
				}
			} else if (in_region == cri.region && !_get_color_region(cri.region).line_only) {
				if (cri.end || _get_color_region(cri.region).eq) {
					in_region = -1;
				}
			}
		}

		if (in_region >= 0 && _get_color_region(in_region).line_only) {
			in_region = -1;
		}

		color_region_cache[i + 1] = in_region;
	}
	return in_region;
}

TextEdit::ColorRegion TextEdit::_get_color_region(int p_region) const {
	if (p_region < 0 || p_region >= color_regions.size()) {
		return ColorRegion();
	}
	return color_regions[p_region];
}

Map<int, TextEdit::Text::ColorRegionInfo> TextEdit::_get_line_color_region_info(int p_line) const {
	if (p_line < 0 || p_line > text.size() - 1) {
		return Map<int, Text::ColorRegionInfo>();
	}
	return text.get_color_region_info(p_line);
}

void TextEdit::clear_colors() {
	keywords.clear();
	member_keywords.clear();
	color_regions.clear();
	color_region_cache.clear();
	syntax_highlighting_cache.clear();
	text.clear_width_cache();
	update();
}

void TextEdit::add_keyword_color(const String &p_keyword, const Color &p_color) {
	keywords[p_keyword] = p_color;
	syntax_highlighting_cache.clear();
	update();
}

bool TextEdit::has_keyword_color(String p_keyword) const {
	return keywords.has(p_keyword);
}

Color TextEdit::get_keyword_color(String p_keyword) const {
	ERR_FAIL_COND_V(!keywords.has(p_keyword), Color());
	return keywords[p_keyword];
}

void TextEdit::add_color_region(const String &p_begin_key, const String &p_end_key, const Color &p_color, bool p_line_only) {
	color_regions.push_back(ColorRegion(p_begin_key, p_end_key, p_color, p_line_only));
	syntax_highlighting_cache.clear();
	text.clear_width_cache();
	update();
}

void TextEdit::add_member_keyword(const String &p_keyword, const Color &p_color) {
	member_keywords[p_keyword] = p_color;
	syntax_highlighting_cache.clear();
	update();
}

bool TextEdit::has_member_color(String p_member) const {
	return member_keywords.has(p_member);
}

Color TextEdit::get_member_color(String p_member) const {
	return member_keywords[p_member];
}
void TextEdit::clear_member_keywords() {
	member_keywords.clear();
	syntax_highlighting_cache.clear();
	update();
}

void TextEdit::set_syntax_coloring(bool p_enabled) {
	syntax_coloring = p_enabled;
	update();
}

bool TextEdit::is_syntax_coloring_enabled() const {
	return syntax_coloring;
}

void TextEdit::set_auto_indent(bool p_auto_indent) {
	auto_indent = p_auto_indent;
}

