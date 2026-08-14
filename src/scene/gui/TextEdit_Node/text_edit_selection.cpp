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

void TextEdit::_update_selection_mode_pointer() {
	selection.drag_attempt = false;
	dragging_selection = true;
	Point2 mp = get_local_mouse_position();

	int row, col;
	_get_mouse_pos(Point2i(mp.x, mp.y), row, col);

	select(selection.selecting_line, selection.selecting_column, row, col);

	cursor_set_line(row, false);
	cursor_set_column(col);
	update();

	click_select_held->start();
}

void TextEdit::_update_selection_mode_word() {
	selection.drag_attempt = false;
	dragging_selection = true;
	Point2 mp = get_local_mouse_position();

	int row, col;
	_get_mouse_pos(Point2i(mp.x, mp.y), row, col);

	String line = text[row];
	int beg = CLAMP(col, 0, line.length());
	// If its the first selection and on whitespace make sure we grab the word instead.
	if (!selection.active) {
		while (beg > 0 && line[beg] <= 32) {
			beg--;
		}
	}
	int end = beg;
	bool symbol = beg < line.length() && _is_symbol(line[beg]);

	// Get the word end and begin points.
	while (beg > 0 && line[beg - 1] > 32 && (symbol == _is_symbol(line[beg - 1]))) {
		beg--;
	}
	while (end < line.length() && line[end + 1] > 32 && (symbol == _is_symbol(line[end + 1]))) {
		end++;
	}
	if (end < line.length()) {
		end += 1;
	}

	// Initial selection.
	if (!selection.active) {
		select(row, beg, row, end);
		selection.selecting_column = beg;
		selection.selected_word_beg = beg;
		selection.selected_word_end = end;
		selection.selected_word_origin = beg;
		cursor_set_line(selection.to_line, false);
		cursor_set_column(selection.to_column);
	} else {
		if ((col <= selection.selected_word_origin && row == selection.selecting_line) || row < selection.selecting_line) {
			selection.selecting_column = selection.selected_word_end;
			select(row, beg, selection.selecting_line, selection.selected_word_end);
			cursor_set_line(selection.from_line, false);
			cursor_set_column(selection.from_column);
		} else {
			selection.selecting_column = selection.selected_word_beg;
			select(selection.selecting_line, selection.selected_word_beg, row, end);
			cursor_set_line(selection.to_line, false);
			cursor_set_column(selection.to_column);
		}
	}

	if (OS::get_singleton()->has_feature("primary_clipboard")) {
		OS::get_singleton()->set_clipboard_primary(get_selection_text());
	}

	update();

	click_select_held->start();
}

void TextEdit::_update_selection_mode_line() {
	selection.drag_attempt = false;
	dragging_selection = true;
	Point2 mp = get_local_mouse_position();

	int row, col;
	_get_mouse_pos(Point2i(mp.x, mp.y), row, col);

	col = 0;
	if (row < selection.selecting_line) {
		// Cursor is above us.
		cursor_set_line(row - 1, false);
		selection.selecting_column = text[selection.selecting_line].length();
	} else {
		// Cursor is below us.
		cursor_set_line(row + 1, false);
		selection.selecting_column = 0;
		col = text[row].length();
	}
	cursor_set_column(0);

	select(selection.selecting_line, selection.selecting_column, row, col);
	if (OS::get_singleton()->has_feature("primary_clipboard")) {
		OS::get_singleton()->set_clipboard_primary(get_selection_text());
	}
	update();

	click_select_held->start();
}

void TextEdit::_update_minimap_hover() {
	const Point2 mp = get_local_mouse_position();
	const int xmargin_end = get_size().width - cache.style_normal->get_margin(MARGIN_RIGHT);

	const bool hovering_sidebar = mp.x > xmargin_end - minimap_width && mp.x < xmargin_end;
	if (!hovering_sidebar) {
		if (hovering_minimap) {
			// Only redraw if the hovering status changed.
			hovering_minimap = false;
			update();
		}

		// Return early to avoid running the operations below when not needed.
		return;
	}

	int row;
	_get_minimap_mouse_row(Point2i(mp.x, mp.y), row);

	const bool new_hovering_minimap = row >= get_first_visible_line() && row <= get_last_full_visible_line();
	if (new_hovering_minimap != hovering_minimap) {
		// Only redraw if the hovering status changed.
		hovering_minimap = new_hovering_minimap;
		update();
	}
}

void TextEdit::_update_minimap_click() {
	Point2 mp = get_local_mouse_position();

	int xmargin_end = get_size().width - cache.style_normal->get_margin(MARGIN_RIGHT);
	if (!dragging_minimap && (mp.x < xmargin_end - minimap_width || mp.y > xmargin_end)) {
		minimap_clicked = false;
		return;
	}
	minimap_clicked = true;
	dragging_minimap = true;

	int row;
	_get_minimap_mouse_row(Point2i(mp.x, mp.y), row);

	if (row >= get_first_visible_line() && (row < get_last_full_visible_line() || row >= (text.size() - 1))) {
		minimap_scroll_ratio = v_scroll->get_as_ratio();
		minimap_scroll_click_pos = mp.y;
		can_drag_minimap = true;
		return;
	}

	int wi;
	int first_line = row - num_lines_from_rows(row, 0, -get_visible_rows() / 2, wi) + 1;
	double delta = get_scroll_pos_for_line(first_line, wi) - get_v_scroll();
	if (delta < 0) {
		_scroll_up(-delta);
	} else {
		_scroll_down(delta);
	}
}

void TextEdit::_update_minimap_drag() {
	if (!can_drag_minimap) {
		return;
	}

	int control_height = _get_control_height();
	int scroll_height = v_scroll->get_max() * (minimap_char_size.y + minimap_line_spacing);
	if (control_height > scroll_height) {
		control_height = scroll_height;
	}

	Point2 mp = get_local_mouse_position();
	double diff = (mp.y - minimap_scroll_click_pos) / control_height;
	v_scroll->set_as_ratio(minimap_scroll_ratio + diff);
}

