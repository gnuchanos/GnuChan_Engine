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

void TextEdit::cut() {
	if (!selection.active) {
		String clipboard = text[cursor.line];
		OS::get_singleton()->set_clipboard(clipboard);
		cursor_set_line(cursor.line);
		cursor_set_column(0);

		if (cursor.line == 0 && get_line_count() > 1) {
			_remove_text(cursor.line, 0, cursor.line + 1, 0);
		} else {
			_remove_text(cursor.line, 0, cursor.line, text[cursor.line].length());
			backspace_at_cursor();
			cursor_set_line(cursor.line + 1);
		}

		update();
		cut_copy_line = clipboard;

	} else {
		String clipboard = _base_get_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
		OS::get_singleton()->set_clipboard(clipboard);

		_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
		cursor_set_line(selection.from_line, false); // Set afterwards else it causes the view to be offset.
		cursor_set_column(selection.from_column);

		selection.active = false;
		selection.selecting_mode = Selection::MODE_NONE;
		update();
		cut_copy_line = "";
	}
}

void TextEdit::copy() {
	if (!selection.active) {
		if (text[cursor.line].length() != 0) {
			String clipboard = _base_get_text(cursor.line, 0, cursor.line, text[cursor.line].length());
			OS::get_singleton()->set_clipboard(clipboard);
			cut_copy_line = clipboard;
		}
	} else {
		String clipboard = _base_get_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
		OS::get_singleton()->set_clipboard(clipboard);
		cut_copy_line = "";
	}
}

void TextEdit::paste() {
	String clipboard = OS::get_singleton()->get_clipboard();

	begin_complex_operation();
	if (selection.active) {
		selection.active = false;
		selection.selecting_mode = Selection::MODE_NONE;
		_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
		cursor_set_line(selection.from_line, false);
		cursor_set_column(selection.from_column);

	} else if (!cut_copy_line.empty() && cut_copy_line == clipboard) {
		cursor_set_column(0);
		String ins = "\n";
		clipboard += ins;
	}

	_insert_text_at_cursor(clipboard);
	end_complex_operation();

	update();
}

void TextEdit::select_all() {
	if (!selecting_enabled) {
		return;
	}

	if (text.size() == 1 && text[0].length() == 0) {
		return;
	}
	selection.active = true;
	selection.from_line = 0;
	selection.from_column = 0;
	selection.selecting_line = 0;
	selection.selecting_column = 0;
	selection.to_line = text.size() - 1;
	selection.to_column = text[selection.to_line].length();
	selection.selecting_mode = Selection::MODE_SHIFT;
	selection.shiftclick_left = true;
	cursor_set_line(selection.to_line, false);
	cursor_set_column(selection.to_column, false);
	update();
}

void TextEdit::deselect() {
	selection.active = false;
	update();
}

void TextEdit::select(int p_from_line, int p_from_column, int p_to_line, int p_to_column) {
	if (!selecting_enabled) {
		return;
	}

	if (p_from_line < 0) {
		p_from_line = 0;
	} else if (p_from_line >= text.size()) {
		p_from_line = text.size() - 1;
	}
	if (p_from_column >= text[p_from_line].length()) {
		p_from_column = text[p_from_line].length();
	}
	if (p_from_column < 0) {
		p_from_column = 0;
	}

	if (p_to_line < 0) {
		p_to_line = 0;
	} else if (p_to_line >= text.size()) {
		p_to_line = text.size() - 1;
	}
	if (p_to_column >= text[p_to_line].length()) {
		p_to_column = text[p_to_line].length();
	}
	if (p_to_column < 0) {
		p_to_column = 0;
	}

	selection.from_line = p_from_line;
	selection.from_column = p_from_column;
	selection.to_line = p_to_line;
	selection.to_column = p_to_column;

	selection.active = true;

	if (selection.from_line == selection.to_line) {
		if (selection.from_column == selection.to_column) {
			selection.active = false;

		} else if (selection.from_column > selection.to_column) {
			selection.shiftclick_left = false;
			SWAP(selection.from_column, selection.to_column);
		} else {
			selection.shiftclick_left = true;
		}
	} else if (selection.from_line > selection.to_line) {
		selection.shiftclick_left = false;
		SWAP(selection.from_line, selection.to_line);
		SWAP(selection.from_column, selection.to_column);
	} else {
		selection.shiftclick_left = true;
	}

	update();
}
void TextEdit::swap_lines(int line1, int line2) {
	String tmp = get_line(line1);
	String tmp2 = get_line(line2);
	set_line(line2, tmp);
	set_line(line1, tmp2);
}
bool TextEdit::is_selection_active() const {
	return selection.active;
}
int TextEdit::get_selection_from_line() const {
	ERR_FAIL_COND_V(!selection.active, -1);
	return selection.from_line;
}
int TextEdit::get_selection_from_column() const {
	ERR_FAIL_COND_V(!selection.active, -1);
	return selection.from_column;
}
int TextEdit::get_selection_to_line() const {
	ERR_FAIL_COND_V(!selection.active, -1);
	return selection.to_line;
}
int TextEdit::get_selection_to_column() const {
	ERR_FAIL_COND_V(!selection.active, -1);
	return selection.to_column;
}
