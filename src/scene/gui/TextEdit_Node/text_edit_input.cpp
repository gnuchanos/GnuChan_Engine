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


void TextEdit::_gui_input(const Ref<InputEvent> &p_gui_input) {
	ERR_FAIL_COND(p_gui_input.is_null());

	double prev_v_scroll = v_scroll->get_value();
	double prev_h_scroll = h_scroll->get_value();

	Ref<InputEventMouseButton> mb = p_gui_input;

	if (mb.is_valid()) {
		if (completion_active && completion_rect.has_point(mb->get_position())) {
			if (!mb->is_pressed()) {
				return;
			}

			if (mb->get_button_index() == BUTTON_WHEEL_UP) {
				if (completion_index > 0) {
					completion_index--;
					completion_current = completion_options[completion_index];
					completion_force_item_center = -1;
					update();
				}
			}
			if (mb->get_button_index() == BUTTON_WHEEL_DOWN) {
				if (completion_index < completion_options.size() - 1) {
					completion_index++;
					completion_current = completion_options[completion_index];
					completion_force_item_center = -1;
					update();
				}
			}

			if (mb->get_button_index() == BUTTON_LEFT) {
				if (completion_force_item_center == -1) {
					completion_force_item_center = completion_index;
				}

				completion_index = CLAMP(completion_line_ofs + (mb->get_position().y - completion_rect.position.y) / get_row_height(), 0, completion_options.size() - 1);
				completion_current = completion_options[completion_index];
				update();
				if (mb->is_doubleclick()) {
					_confirm_completion();
				}
			}
			return;
		} else {
			_cancel_completion();
			_cancel_code_hint();
		}

		if (mb->is_pressed()) {
			if (mb->get_button_index() == BUTTON_WHEEL_UP && !mb->get_command()) {
				if (mb->get_shift()) {
					h_scroll->set_value(h_scroll->get_value() - (100 * mb->get_factor()));
				} else if (mb->get_alt()) {
					// Scroll 5 times as fast as normal (like in Visual Studio Code).
					_scroll_up(15 * mb->get_factor());
				} else if (v_scroll->is_visible()) {
					// Scroll 3 lines.
					_scroll_up(3 * mb->get_factor());
				}
			}
			if (mb->get_button_index() == BUTTON_WHEEL_DOWN && !mb->get_command()) {
				if (mb->get_shift()) {
					h_scroll->set_value(h_scroll->get_value() + (100 * mb->get_factor()));
				} else if (mb->get_alt()) {
					// Scroll 5 times as fast as normal (like in Visual Studio Code).
					_scroll_down(15 * mb->get_factor());
				} else if (v_scroll->is_visible()) {
					// Scroll 3 lines.
					_scroll_down(3 * mb->get_factor());
				}
			}
			if (mb->get_button_index() == BUTTON_WHEEL_LEFT) {
				h_scroll->set_value(h_scroll->get_value() - (100 * mb->get_factor()));
			}
			if (mb->get_button_index() == BUTTON_WHEEL_RIGHT) {
				h_scroll->set_value(h_scroll->get_value() + (100 * mb->get_factor()));
			}
			if (mb->get_button_index() == BUTTON_LEFT) {
				_reset_caret_blink_timer();

				int row, col;
				_get_mouse_pos(Point2i(mb->get_position().x, mb->get_position().y), row, col);

				// Toggle breakpoint on gutter click.
				if (draw_breakpoint_gutter) {
					int gutter = cache.style_normal->get_margin(MARGIN_LEFT);
					if (mb->get_position().x > gutter - 6 && mb->get_position().x <= gutter + cache.breakpoint_gutter_width - 3) {
						set_line_as_breakpoint(row, !is_line_set_as_breakpoint(row));
						emit_signal("breakpoint_toggled", row);
						return;
					}
				}

				// Emit info clicked.
				if (draw_info_gutter && text.has_info_icon(row)) {
					int left_margin = cache.style_normal->get_margin(MARGIN_LEFT);
					int gutter_left = left_margin + cache.breakpoint_gutter_width;
					if (mb->get_position().x > gutter_left - 6 && mb->get_position().x <= gutter_left + cache.info_gutter_width - 3) {
						emit_signal("info_clicked", row, text.get_info(row));
						return;
					}
				}

				// Toggle fold on gutter click if can.
				if (draw_fold_gutter) {
					int left_margin = cache.style_normal->get_margin(MARGIN_LEFT);
					int gutter_left = left_margin + cache.breakpoint_gutter_width + cache.line_number_w + cache.info_gutter_width;
					if (mb->get_position().x > gutter_left - 6 && mb->get_position().x <= gutter_left + cache.fold_gutter_width - 3) {
						if (is_folded(row)) {
							unfold_line(row);
						} else if (can_fold(row)) {
							fold_line(row);
						}
						return;
					}
				}

				// Unfold on folded icon click.
				if (is_folded(row)) {
					int line_width = text.get_line_width(row);
					line_width += cache.style_normal->get_margin(MARGIN_LEFT) + cache.line_number_w + cache.breakpoint_gutter_width + cache.info_gutter_width + cache.fold_gutter_width - cursor.x_ofs;
					if (mb->get_position().x > line_width - 3 && mb->get_position().x <= line_width + cache.folded_eol_icon->get_width() + 3) {
						unfold_line(row);
						return;
					}
				}

				// minimap
				if (draw_minimap) {
					_update_minimap_click();
					if (dragging_minimap) {
						return;
					}
				}

				int prev_col = cursor.column;
				int prev_line = cursor.line;

				cursor_set_line(row, false, false);
				cursor_set_column(col);
				selection.drag_attempt = false;

				if (mb->get_shift() && (cursor.column != prev_col || cursor.line != prev_line)) {
					if (!selection.active) {
						selection.active = true;
						selection.selecting_mode = Selection::MODE_POINTER;
						selection.from_column = prev_col;
						selection.from_line = prev_line;
						selection.to_column = cursor.column;
						selection.to_line = cursor.line;

						if (selection.from_line > selection.to_line || (selection.from_line == selection.to_line && selection.from_column > selection.to_column)) {
							SWAP(selection.from_column, selection.to_column);
							SWAP(selection.from_line, selection.to_line);
							selection.shiftclick_left = false;
						} else {
							selection.shiftclick_left = true;
						}
						selection.selecting_line = prev_line;
						selection.selecting_column = prev_col;
						update();
					} else {
						if (cursor.line < selection.selecting_line || (cursor.line == selection.selecting_line && cursor.column < selection.selecting_column)) {
							if (selection.shiftclick_left) {
								SWAP(selection.from_column, selection.to_column);
								SWAP(selection.from_line, selection.to_line);
								selection.shiftclick_left = !selection.shiftclick_left;
							}
							selection.from_column = cursor.column;
							selection.from_line = cursor.line;

						} else if (cursor.line > selection.selecting_line || (cursor.line == selection.selecting_line && cursor.column > selection.selecting_column)) {
							if (!selection.shiftclick_left) {
								SWAP(selection.from_column, selection.to_column);
								SWAP(selection.from_line, selection.to_line);
								selection.shiftclick_left = !selection.shiftclick_left;
							}
							selection.to_column = cursor.column;
							selection.to_line = cursor.line;

						} else {
							selection.active = false;
						}

						update();
					}
				} else if (drag_and_drop_selection_enabled && is_mouse_over_selection()) {
					selection.selecting_mode = Selection::MODE_NONE;
					selection.drag_attempt = true;
				} else {
					selection.active = false;
					selection.selecting_mode = Selection::MODE_POINTER;
					selection.selecting_line = row;
					selection.selecting_column = col;
				}

				const int triple_click_timeout = 600;
				const int triple_click_tolerance = 5;

				if (!mb->is_doubleclick() && (OS::get_singleton()->get_ticks_msec() - last_dblclk) < triple_click_timeout && mb->get_position().distance_to(last_dblclk_pos) < triple_click_tolerance) {
					// Triple-click select line.
					selection.selecting_mode = Selection::MODE_LINE;
					_update_selection_mode_line();
					last_dblclk = 0;
				} else if (mb->is_doubleclick() && text[cursor.line].length()) {
					// Double-click select word.
					selection.selecting_mode = Selection::MODE_WORD;
					_update_selection_mode_word();
					last_dblclk = OS::get_singleton()->get_ticks_msec();
					last_dblclk_pos = mb->get_position();
				}

				update();
			}

			if (is_middle_mouse_paste_enabled() && mb->get_button_index() == BUTTON_MIDDLE && !readonly && OS::get_singleton()->has_feature("primary_clipboard")) {
				String paste_buffer = OS::get_singleton()->get_clipboard_primary();

				int row, col;
				_get_mouse_pos(Point2i(mb->get_position().x, mb->get_position().y), row, col);
				begin_complex_operation();

				deselect();
				cursor_set_line(row, true, false);
				cursor_set_column(col);
				if (!paste_buffer.empty()) {
					_insert_text_at_cursor(paste_buffer);
				}
				end_complex_operation();

				grab_focus();
				update();
			}

			if (mb->get_button_index() == BUTTON_RIGHT && context_menu_enabled) {
				_reset_caret_blink_timer();

				int row, col;
				_get_mouse_pos(Point2i(mb->get_position().x, mb->get_position().y), row, col);

				if (is_right_click_moving_caret()) {
					if (is_selection_active()) {
						int from_line = get_selection_from_line();
						int to_line = get_selection_to_line();
						int from_column = get_selection_from_column();
						int to_column = get_selection_to_column();

						if (row < from_line || row > to_line || (row == from_line && col < from_column) || (row == to_line && col > to_column)) {
							// Right click is outside the selected text.
							deselect();
						}
					}
					if (!is_selection_active()) {
						cursor_set_line(row, true, false);
						cursor_set_column(col);
					}
				}

				popup_show = true;
				if (!readonly) {
					menu->set_item_disabled(menu->get_item_index(MENU_UNDO), !has_undo());
					menu->set_item_disabled(menu->get_item_index(MENU_REDO), !has_redo());
				}

				menu->set_position(get_global_transform().xform(get_local_mouse_position()));
				menu->set_size(Vector2(1, 1));
				menu->set_scale(get_global_transform().get_scale());
				menu->popup();
			}
		} else {
			if (mb->get_button_index() == BUTTON_LEFT) {
				if (selection.drag_attempt && selection.selecting_mode == Selection::MODE_NONE && is_mouse_over_selection()) {
					selection.active = false;
				}
				if (mb->get_command() && highlighted_word != String()) {
					int row, col;
					_get_mouse_pos(Point2i(mb->get_position().x, mb->get_position().y), row, col);

					emit_signal("symbol_lookup", highlighted_word, row, col);
					return;
				}

				dragging_minimap = false;
				dragging_selection = false;
				can_drag_minimap = false;
				click_select_held->stop();
				if (!drag_action) {
					selection.drag_attempt = false;
				}
				if (OS::get_singleton()->has_feature("primary_clipboard")) {
					OS::get_singleton()->set_clipboard_primary(get_selection_text());
				}
			}

			// Notify to show soft keyboard.
			notification(NOTIFICATION_FOCUS_ENTER);
		}
	}

	const Ref<InputEventPanGesture> pan_gesture = p_gui_input;
	if (pan_gesture.is_valid()) {
		const real_t delta = pan_gesture->get_delta().y;
		if (delta < 0) {
			_scroll_up(-delta);
		} else {
			_scroll_down(delta);
		}
		h_scroll->set_value(h_scroll->get_value() + pan_gesture->get_delta().x * 100);
		if (v_scroll->get_value() != prev_v_scroll || h_scroll->get_value() != prev_h_scroll) {
			accept_event(); // Accept event if scroll changed.
		}

		return;
	}

	Ref<InputEventMouseMotion> mm = p_gui_input;

	if (mm.is_valid()) {
		if (select_identifiers_enabled) {
			if (!dragging_minimap && !dragging_selection && mm->get_command() && mm->get_button_mask() == 0) {
				String new_word = get_word_at_pos(mm->get_position());
				if (new_word != highlighted_word) {
					highlighted_word = new_word;
					update();
				}
			} else {
				if (highlighted_word != String()) {
					highlighted_word = String();
					update();
				}
			}
		}

		if (draw_minimap && !dragging_selection) {
			_update_minimap_hover();
		}

		if (mm->get_button_mask() & BUTTON_MASK_LEFT && get_viewport()->gui_get_drag_data() == Variant()) { // Ignore if dragging.
			_reset_caret_blink_timer();

			if (draw_minimap && !dragging_selection) {
				_update_minimap_drag();
			}

			if (!dragging_minimap) {
				switch (selection.selecting_mode) {
					case Selection::MODE_POINTER: {
						_update_selection_mode_pointer();
					} break;
					case Selection::MODE_WORD: {
						_update_selection_mode_word();
					} break;
					case Selection::MODE_LINE: {
						_update_selection_mode_line();
					} break;
					default: {
						break;
					}
				}
			}
		}

		if (drag_action && can_drop_data(mm->get_position(), get_viewport()->gui_get_drag_data())) {
			drag_caret_force_displayed = true;
			Point2 mp = get_local_mouse_position();
			int row, col;
			_get_mouse_pos(Point2i(mp.x, mp.y), row, col);
			cursor_set_line(row, true);
			cursor_set_column(col);
			if (!scroll_past_end_of_file_enabled) {
				if (row <= get_first_visible_line()) {
					_scroll_lines_up();
				} else if (row >= get_last_full_visible_line()) {
					_scroll_lines_down();
				}
			}
			dragging_selection = true;
			update();
		}
	}

	if (v_scroll->get_value() != prev_v_scroll || h_scroll->get_value() != prev_h_scroll) {
		accept_event(); // Accept event if scroll changed.
	}

	Ref<InputEventKey> k = p_gui_input;

	if (k.is_valid()) {
		k = k->duplicate(); // It will be modified later on.

#ifdef OSX_ENABLED
		if (k->get_scancode() == KEY_META) {
#else
		if (k->get_scancode() == KEY_CONTROL) {

#endif
			if (select_identifiers_enabled) {
				if (k->is_pressed() && !dragging_minimap && !dragging_selection) {
					highlighted_word = get_word_at_pos(get_local_mouse_position());
					update();

				} else {
					highlighted_word = String();
					update();
				}
			}
		}

		if (!k->is_pressed()) {
			return;
		}

		if (completion_active) {
			if (readonly) {
				return;
			}

			bool valid = true;
			if (k->get_command() || k->get_metakey()) {
				valid = false;
			}

			if (valid) {
				if (!k->get_alt()) {
					if (k->get_scancode() == KEY_UP) {
						if (completion_index > 0) {
							completion_index--;
						} else {
							completion_index = completion_options.size() - 1;
						}
						completion_current = completion_options[completion_index];
						completion_force_item_center = -1;
						update();

						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_DOWN) {
						if (completion_index < completion_options.size() - 1) {
							completion_index++;
						} else {
							completion_index = 0;
						}
						completion_current = completion_options[completion_index];
						completion_force_item_center = -1;
						update();

						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_PAGEUP) {
						completion_index -= get_constant("completion_lines");
						if (completion_index < 0) {
							completion_index = 0;
						}
						completion_current = completion_options[completion_index];
						completion_force_item_center = -1;
						update();
						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_PAGEDOWN) {
						completion_index += get_constant("completion_lines");
						if (completion_index >= completion_options.size()) {
							completion_index = completion_options.size() - 1;
						}
						completion_current = completion_options[completion_index];
						completion_force_item_center = -1;
						update();
						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_HOME && completion_index > 0) {
						completion_index = 0;
						completion_current = completion_options[completion_index];
						completion_force_item_center = -1;
						update();
						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_END && completion_index < completion_options.size() - 1) {
						completion_index = completion_options.size() - 1;
						completion_current = completion_options[completion_index];
						completion_force_item_center = -1;
						update();
						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_KP_ENTER || k->get_scancode() == KEY_ENTER || k->get_scancode() == KEY_TAB) {
						_confirm_completion();
						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_BACKSPACE) {
						_reset_caret_blink_timer();

						backspace_at_cursor();
						_update_completion_candidates();
						accept_event();
						return;
					}

					if (k->get_scancode() == KEY_SHIFT) {
						accept_event();
						return;
					}
				}

				if (k->get_unicode() > 32) {
					_reset_caret_blink_timer();

					const CharType chr[2] = { (CharType)k->get_unicode(), 0 };
					if (auto_brace_completion_enabled && _is_pair_symbol(chr[0])) {
						_consume_pair_symbol(chr[0]);
					} else {
						// Remove the old character if in insert mode.
						if (insert_mode) {
							begin_complex_operation();

							// Make sure we don't try and remove empty space.
							if (cursor.column < get_line(cursor.line).length()) {
								_remove_text(cursor.line, cursor.column, cursor.line, cursor.column + 1);
							}
						}

						_insert_text_at_cursor(chr);

						if (insert_mode) {
							end_complex_operation();
						}
					}
					_update_completion_candidates();
					accept_event();

					return;
				}
			}

			_cancel_completion();
		}

		/* TEST CONTROL FIRST! */

		// Some remaps for duplicate functions.
		if (k->get_command() && !k->get_shift() && !k->get_alt() && !k->get_metakey() && k->get_scancode() == KEY_INSERT) {
			k->set_scancode(KEY_C);
		}
		if (!k->get_command() && k->get_shift() && !k->get_alt() && !k->get_metakey() && k->get_scancode() == KEY_INSERT) {
			k->set_scancode(KEY_V);
			k->set_command(true);
			k->set_shift(false);
		}
#ifdef APPLE_STYLE_KEYS
		if (k->get_control() && !k->get_shift() && !k->get_alt() && !k->get_command()) {
			uint32_t remap_key = KEY_UNKNOWN;
			switch (k->get_scancode()) {
				case KEY_F: {
					remap_key = KEY_RIGHT;
				} break;
				case KEY_B: {
					remap_key = KEY_LEFT;
				} break;
				case KEY_P: {
					remap_key = KEY_UP;
				} break;
				case KEY_N: {
					remap_key = KEY_DOWN;
				} break;
				case KEY_D: {
					remap_key = KEY_DELETE;
				} break;
				case KEY_H: {
					remap_key = KEY_BACKSPACE;
				} break;
			}

			if (remap_key != KEY_UNKNOWN) {
				k->set_scancode(remap_key);
				k->set_control(false);
			}
		}
#endif

		_reset_caret_blink_timer();

		// Save here for insert mode as well as arrow navigation, just in case it is cleared in the following section.
		bool had_selection = selection.active;

		// Stuff to do when selection is active.
		if (!readonly && selection.active) {
			bool clear = false;
			bool unselect = false;
			bool dobreak = false;

			switch (k->get_scancode()) {
				case KEY_TAB: {
					if (k->get_shift()) {
						indent_left();
					} else {
						indent_right();
					}
					dobreak = true;
					accept_event();
				} break;
				case KEY_X:
				case KEY_C:
					// Special keys often used with control, wait.
					clear = (!k->get_command() || k->get_shift() || k->get_alt());
					break;
				case KEY_DELETE:
					if (!k->get_shift()) {
						accept_event();
						clear = true;
						dobreak = true;
					} else if (k->get_command() || k->get_alt()) {
						dobreak = true;
					}
					break;
				case KEY_BACKSPACE:
					accept_event();
					clear = true;
					dobreak = true;
					break;
				case KEY_LEFT:
				case KEY_RIGHT:
				case KEY_UP:
				case KEY_DOWN:
				case KEY_PAGEUP:
				case KEY_PAGEDOWN:
				case KEY_HOME:
				case KEY_END:
					// Ignore arrows if any modifiers are held (shift = selecting, others may be used for editor hotkeys).
					if (k->get_command() || k->get_shift() || k->get_alt()) {
						break;
					}
					unselect = true;
					break;

				default:
					if (k->get_unicode() >= 32 && !k->get_command() && !k->get_alt() && !k->get_metakey()) {
						clear = true;
					}
					if (auto_brace_completion_enabled && _is_pair_left_symbol(k->get_unicode())) {
						clear = false;
					}
			}

			if (unselect) {
				selection.active = false;
				selection.selecting_mode = Selection::MODE_NONE;
				update();
			}
			if (clear) {
				if (!dobreak) {
					begin_complex_operation();
				}
				selection.active = false;
				update();
				_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
				cursor_set_line(selection.from_line, false, false);
				cursor_set_column(selection.from_column);
				update();
			}
			if (dobreak) {
				return;
			}
		}

		selection.selecting_text = false;

		bool scancode_handled = true;

		// Special scancode test.

		switch (k->get_scancode()) {
			case KEY_KP_ENTER:
			case KEY_ENTER: {
				if (readonly) {
					break;
				}

				String ins = "\n";

				// Keep indentation.
				int space_count = 0;
				for (int i = 0; i < cursor.column; i++) {
					if (text[cursor.line][i] == '\t') {
						if (indent_using_spaces) {
							ins += space_indent;
						} else {
							ins += "\t";
						}
						space_count = 0;
					} else if (text[cursor.line][i] == ' ') {
						space_count++;

						if (space_count == indent_size) {
							if (indent_using_spaces) {
								ins += space_indent;
							} else {
								ins += "\t";
							}
							space_count = 0;
						}
					} else {
						break;
					}
				}

				if (is_folded(cursor.line)) {
					unfold_line(cursor.line);
				}

				bool brace_indent = false;

				// No need to indent if we are going upwards.
				if (auto_indent && !(k->get_command() && k->get_shift())) {
					// Indent once again if previous line will end with ':','{','[','(' and the line is not a comment
					// (i.e. colon/brace precedes current cursor position).
					if (cursor.column > 0) {
						const Map<int, Text::ColorRegionInfo> &cri_map = text.get_color_region_info(cursor.line);
						bool indent_char_found = false;
						bool should_indent = false;
						char indent_char = ':';
						char c = text[cursor.line][cursor.column];

						for (int i = 0; i < cursor.column; i++) {
							c = text[cursor.line][i];
							switch (c) {
								case ':':
								case '{':
								case '[':
								case '(':
									indent_char_found = true;
									should_indent = true;
									indent_char = c;
									continue;
							}

							if (indent_char_found && cri_map.has(i) && (color_regions[cri_map[i].region].begin_key == "#" || color_regions[cri_map[i].region].begin_key == "//")) {
								should_indent = true;
								break;
							} else if (indent_char_found && !_is_whitespace(c)) {
								should_indent = false;
								indent_char_found = false;
							}
						}

						if (!is_line_comment(cursor.line) && should_indent) {
							if (indent_using_spaces) {
								ins += space_indent;
							} else {
								ins += "\t";
							}

							// No need to move the brace below if we are not taking the text with us.
							char closing_char = _get_right_pair_symbol(indent_char);
							if ((closing_char != 0) && (closing_char == text[cursor.line][cursor.column]) && !k->get_command()) {
								brace_indent = true;
								ins += "\n" + ins.substr(1, ins.length() - 2);
							}
						}
					}
				}
				begin_complex_operation();
				bool first_line = false;
				if (k->get_command()) {
					if (k->get_shift()) {
						if (cursor.line > 0) {
							cursor_set_line(cursor.line - 1);
							cursor_set_column(text[cursor.line].length());
						} else {
							cursor_set_column(0);
							first_line = true;
						}
					} else {
						cursor_set_column(text[cursor.line].length());
					}
				}

				insert_text_at_cursor(ins);

				if (first_line) {
					cursor_set_line(0);
				} else if (brace_indent) {
					cursor_set_line(cursor.line - 1);
					cursor_set_column(text[cursor.line].length());
				}
				end_complex_operation();
			} break;
			case KEY_ESCAPE: {
				if (completion_hint != "") {
					completion_hint = "";
					update();
				} else {
					scancode_handled = false;
				}
			} break;
			case KEY_TAB: {
				if (k->get_command()) {
					break; // Avoid tab when command.
				}

				if (readonly) {
					break;
				}

				if (is_selection_active()) {
					if (k->get_shift()) {
						indent_left();
					} else {
						indent_right();
					}
				} else {
					if (k->get_shift()) {
						// Simple unindent.
						int cc = cursor.column;
						const String &line = text[cursor.line];

						int left = _find_first_non_whitespace_column_of_line(line);
						cc = MIN(cc, left);

						while (cc < indent_size && cc < left && line[cc] == ' ') {
							cc++;
						}

						if (cc > 0 && cc <= text[cursor.line].length()) {
							if (text[cursor.line][cc - 1] == '\t') {
								// Tabs unindentation.
								_remove_text(cursor.line, cc - 1, cursor.line, cc);
								if (cursor.column >= left) {
									cursor_set_column(MAX(0, cursor.column - 1));
								}
								update();
							} else {
								// Spaces unindentation.
								int spaces_to_remove = _calculate_spaces_till_next_left_indent(cc);
								if (spaces_to_remove > 0) {
									_remove_text(cursor.line, cc - spaces_to_remove, cursor.line, cc);
									if (cursor.column > left - spaces_to_remove) { // Inside text?
										cursor_set_column(MAX(0, cursor.column - spaces_to_remove));
									}
									update();
								}
							}
						} else if (cc == 0 && line.length() > 0 && line[0] == '\t') {
							_remove_text(cursor.line, 0, cursor.line, 1);
							update();
						}
					} else {
						// Simple indent.
						if (indent_using_spaces) {
							// Insert only as much spaces as needed till next indentation level.
							int spaces_to_add = _calculate_spaces_till_next_right_indent(cursor.column);
							String indent_to_insert = String();
							for (int i = 0; i < spaces_to_add; i++) {
								indent_to_insert = ' ' + indent_to_insert;
							}
							_insert_text_at_cursor(indent_to_insert);
						} else {
							_insert_text_at_cursor("\t");
						}
					}
				}

			} break;
			case KEY_BACKSPACE: {
				if (readonly) {
					break;
				}

#ifdef APPLE_STYLE_KEYS
				if (k->get_alt() && cursor.column > 1) {
#else
				if (k->get_alt()) {
					scancode_handled = false;
					break;
				} else if (k->get_command() && cursor.column > 1) {
#endif
					int line = cursor.line;
					int column = cursor.column;

					// Check if we are removing a single whitespace, if so remove it and the next char type,
					// else we just remove the whitespace.
					bool only_whitespace = false;
					if (_is_whitespace(text[line][column - 1]) && _is_whitespace(text[line][column - 2])) {
						only_whitespace = true;
					} else if (_is_whitespace(text[line][column - 1])) {
						// Remove the single whitespace.
						column--;
					}

					// Check if its a text char.
					bool only_char = (_is_text_char(text[line][column - 1]) && !only_whitespace);

					// If its not whitespace or char then symbol.
					bool only_symbols = !(only_whitespace || only_char);

					while (column > 0) {
						bool is_whitespace = _is_whitespace(text[line][column - 1]);
						bool is_text_char = _is_text_char(text[line][column - 1]);

						if (only_whitespace && !is_whitespace) {
							break;
						} else if (only_char && !is_text_char) {
							break;
						} else if (only_symbols && (is_whitespace || is_text_char)) {
							break;
						}
						column--;
					}

					_remove_text(line, column, cursor.line, cursor.column);

					cursor_set_line(line);
					cursor_set_column(column);

#ifdef APPLE_STYLE_KEYS
				} else if (k->get_command()) {
					int cursor_current_column = cursor.column;
					cursor.column = 0;
					_remove_text(cursor.line, 0, cursor.line, cursor_current_column);
#endif
				} else {
					if (cursor.line > 0 && is_line_hidden(cursor.line - 1)) {
						unfold_line(cursor.line - 1);
					}
					backspace_at_cursor();
				}

			} break;
			case KEY_KP_4: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_LEFT: {
				if (k->get_shift()) {
					_pre_shift_selection();
				} else if (had_selection && !k->get_command() && !k->get_alt()) {
					cursor_set_line(selection.from_line);
					cursor_set_column(selection.from_column);
					deselect();
					break;
#ifdef APPLE_STYLE_KEYS
				} else {
#else
				} else if (!k->get_alt()) {
#endif
					deselect();
				}

#ifdef APPLE_STYLE_KEYS
				if (k->get_command()) {
					// Start at first column (it's slightly faster that way) and look for the first non-whitespace character.
					int new_cursor_pos = 0;
					for (int i = 0; i < text[cursor.line].length(); ++i) {
						if (!_is_whitespace(text[cursor.line][i])) {
							new_cursor_pos = i;
							break;
						}
					}
					if (new_cursor_pos == cursor.column) {
						// We're already at the first text character, so move to the very beginning of the line.
						cursor_set_column(0);
					} else {
						// We're somewhere to the right of the first text character; move to the first one.
						cursor_set_column(new_cursor_pos);
					}
				} else if (k->get_alt()) {
#else
				if (k->get_alt()) {
					scancode_handled = false;
					break;
				} else if (k->get_command()) {
#endif
					int cc = cursor.column;

					if (cc == 0 && cursor.line > 0) {
						cursor_set_line(cursor.line - 1);
						cursor_set_column(text[cursor.line].length());
					} else {
						bool prev_char = false;

						while (cc > 0) {
							bool ischar = _is_text_char(text[cursor.line][cc - 1]);

							if (prev_char && !ischar) {
								break;
							}

							prev_char = ischar;
							cc--;
						}
						cursor_set_column(cc);
					}

				} else if (cursor.column == 0) {
					if (cursor.line > 0) {
						cursor_set_line(cursor.line - num_lines_from(CLAMP(cursor.line - 1, 0, text.size() - 1), -1));
						cursor_set_column(text[cursor.line].length());
					}
				} else {
					cursor_set_column(cursor_get_column() - 1);
				}

				if (k->get_shift()) {
					_post_shift_selection();
				}

			} break;
			case KEY_KP_6: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_RIGHT: {
				if (k->get_shift()) {
					_pre_shift_selection();
				} else if (had_selection && !k->get_command() && !k->get_alt()) {
					cursor_set_line(selection.to_line);
					cursor_set_column(selection.to_column);
					deselect();
					break;
#ifdef APPLE_STYLE_KEYS
				} else {
#else
				} else if (!k->get_alt()) {
#endif
					deselect();
				}

#ifdef APPLE_STYLE_KEYS
				if (k->get_command()) {
					cursor_set_column(text[cursor.line].length());
				} else if (k->get_alt()) {
#else
				if (k->get_alt()) {
					scancode_handled = false;
					break;
				} else if (k->get_command()) {
#endif
					int cc = cursor.column;

					if (cc == text[cursor.line].length() && cursor.line < text.size() - 1) {
						cursor_set_line(cursor.line + 1);
						cursor_set_column(0);
					} else {
						bool prev_char = false;

						while (cc < text[cursor.line].length()) {
							bool ischar = _is_text_char(text[cursor.line][cc]);

							if (prev_char && !ischar) {
								break;
							}
							prev_char = ischar;
							cc++;
						}
						cursor_set_column(cc);
					}

				} else if (cursor.column == text[cursor.line].length()) {
					if (cursor.line < text.size() - 1) {
						cursor_set_line(cursor_get_line() + num_lines_from(CLAMP(cursor.line + 1, 0, text.size() - 1), 1), true, false);
						cursor_set_column(0);
					}
				} else {
					cursor_set_column(cursor_get_column() + 1);
				}

				if (k->get_shift()) {
					_post_shift_selection();
				}

			} break;
			case KEY_KP_8: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_UP: {
				if (k->get_alt()) {
					scancode_handled = false;
					break;
				}
#ifndef APPLE_STYLE_KEYS
				if (k->get_command()) {
#else
				if (k->get_command() && k->get_alt()) {
#endif
					_scroll_lines_up();
					break;
				}

				if (k->get_shift()) {
					_pre_shift_selection();
				}

#ifdef APPLE_STYLE_KEYS
				if (k->get_command()) {
					cursor_set_line(0);
				} else
#endif
				{
					int cur_wrap_index = get_cursor_wrap_index();
					if (cur_wrap_index > 0) {
						cursor_set_line(cursor.line, true, false, cur_wrap_index - 1);
					} else if (cursor.line == 0) {
						cursor_set_column(0);
					} else {
						int new_line = cursor.line - num_lines_from(cursor.line - 1, -1);
						if (line_wraps(new_line)) {
							cursor_set_line(new_line, true, false, times_line_wraps(new_line));
						} else {
							cursor_set_line(new_line, true, false);
						}
					}
				}

				if (k->get_shift()) {
					_post_shift_selection();
				}
				_cancel_code_hint();

			} break;
			case KEY_KP_2: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_DOWN: {
				if (k->get_alt()) {
					scancode_handled = false;
					break;
				}
#ifndef APPLE_STYLE_KEYS
				if (k->get_command()) {
#else
				if (k->get_command() && k->get_alt()) {
#endif
					_scroll_lines_down();
					break;
				}

				if (k->get_shift()) {
					_pre_shift_selection();
				}

#ifdef APPLE_STYLE_KEYS
				if (k->get_command()) {
					cursor_set_line(get_last_unhidden_line(), true, false, 9999);
				} else
#endif
				{
					int cur_wrap_index = get_cursor_wrap_index();
					if (cur_wrap_index < times_line_wraps(cursor.line)) {
						cursor_set_line(cursor.line, true, false, cur_wrap_index + 1);
					} else if (cursor.line == get_last_unhidden_line()) {
						cursor_set_column(text[cursor.line].length());
					} else {
						int new_line = cursor.line + num_lines_from(CLAMP(cursor.line + 1, 0, text.size() - 1), 1);
						cursor_set_line(new_line, true, false, 0);
					}
				}

				if (k->get_shift()) {
					_post_shift_selection();
				}
				_cancel_code_hint();

			} break;
			case KEY_DELETE: {
				if (readonly) {
					break;
				}

				if (k->get_shift() && !k->get_command() && !k->get_alt() && is_shortcut_keys_enabled()) {
					cut();
					break;
				}

				int curline_len = text[cursor.line].length();

				if (cursor.line == text.size() - 1 && cursor.column == curline_len) {
					break; // Nothing to do.
				}

				int next_line = cursor.column < curline_len ? cursor.line : cursor.line + 1;
				int next_column;

#ifdef APPLE_STYLE_KEYS
				if (k->get_alt() && cursor.column < curline_len - 1) {
#else
				if (k->get_alt()) {
					scancode_handled = false;
					break;
				} else if (k->get_command() && cursor.column < curline_len - 1) {
#endif

					int line = cursor.line;
					int column = cursor.column;

					// Check if we are removing a single whitespace, if so remove it and the next char type,
					// else we just remove the whitespace.
					bool only_whitespace = false;
					if (_is_whitespace(text[line][column]) && _is_whitespace(text[line][column + 1])) {
						only_whitespace = true;
					} else if (_is_whitespace(text[line][column])) {
						// Remove the single whitespace.
						column++;
					}

					// Check if its a text char.
					bool only_char = (_is_text_char(text[line][column]) && !only_whitespace);

					// If its not whitespace or char then symbol.
					bool only_symbols = !(only_whitespace || only_char);

					while (column < curline_len) {
						bool is_whitespace = _is_whitespace(text[line][column]);
						bool is_text_char = _is_text_char(text[line][column]);

						if (only_whitespace && !is_whitespace) {
							break;
						} else if (only_char && !is_text_char) {
							break;
						} else if (only_symbols && (is_whitespace || is_text_char)) {
							break;
						}
						column++;
					}

					next_line = line;
					next_column = column;
#ifdef APPLE_STYLE_KEYS
				} else if (k->get_command()) {
					next_column = curline_len;
					next_line = cursor.line;
#endif
				} else {
					next_column = cursor.column < curline_len ? (cursor.column + 1) : 0;
				}

				_remove_text(cursor.line, cursor.column, next_line, next_column);
				update();

			} break;
			case KEY_KP_7: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_HOME: {
#ifdef APPLE_STYLE_KEYS
				if (k->get_shift()) {
					_pre_shift_selection();
				}

				cursor_set_line(0);

				if (k->get_shift()) {
					_post_shift_selection();
				} else if (k->get_command() || k->get_control()) {
					deselect();
				}
#else
				if (k->get_shift()) {
					_pre_shift_selection();
				}

				if (k->get_command()) {
					cursor_set_line(0);
					cursor_set_column(0);
				} else {
					// Move cursor column to start of wrapped row and then to start of text.
					Vector<String> rows = get_wrap_rows_text(cursor.line);
					int wi = get_cursor_wrap_index();
					int row_start_col = 0;
					for (int i = 0; i < wi; i++) {
						row_start_col += rows[i].length();
					}
					if (cursor.column == row_start_col || wi == 0) {
						// Compute whitespace symbols seq length.
						int current_line_whitespace_len = 0;
						while (current_line_whitespace_len < text[cursor.line].length()) {
							CharType c = text[cursor.line][current_line_whitespace_len];
							if (c != '\t' && c != ' ') {
								break;
							}
							current_line_whitespace_len++;
						}

						if (cursor_get_column() == current_line_whitespace_len) {
							cursor_set_column(0);
						} else {
							cursor_set_column(current_line_whitespace_len);
						}
					} else {
						cursor_set_column(row_start_col);
					}
				}

				if (k->get_shift()) {
					_post_shift_selection();
				} else if (k->get_command() || k->get_control()) {
					deselect();
				}
				_cancel_completion();
				completion_hint = "";
#endif
			} break;
			case KEY_KP_1: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_END: {
#ifdef APPLE_STYLE_KEYS
				if (k->get_shift()) {
					_pre_shift_selection();
				}

				cursor_set_line(get_last_unhidden_line(), true, false, 9999);

				if (k->get_shift()) {
					_post_shift_selection();
				} else if (k->get_command() || k->get_control()) {
					deselect();
				}
#else
				if (k->get_shift()) {
					_pre_shift_selection();
				}

				if (k->get_command()) {
					cursor_set_line(get_last_unhidden_line(), true, false, 9999);
				}

				// Move cursor column to end of wrapped row and then to end of text.
				Vector<String> rows = get_wrap_rows_text(cursor.line);
				int wi = get_cursor_wrap_index();
				int row_end_col = -1;
				for (int i = 0; i < wi + 1; i++) {
					row_end_col += rows[i].length();
				}
				if (wi == rows.size() - 1 || cursor.column == row_end_col) {
					cursor_set_column(text[cursor.line].length());
				} else {
					cursor_set_column(row_end_col);
				}

				if (k->get_shift()) {
					_post_shift_selection();
				} else if (k->get_command() || k->get_control()) {
					deselect();
				}

				_cancel_completion();
				completion_hint = "";
#endif
			} break;
			case KEY_KP_9: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_PAGEUP: {
				if (k->get_shift()) {
					_pre_shift_selection();
				}

				int wi;
				int n_line = cursor.line - num_lines_from_rows(cursor.line, get_cursor_wrap_index(), -get_visible_rows(), wi) + 1;
				cursor_set_line(n_line, true, false, wi);

				if (k->get_shift()) {
					_post_shift_selection();
				}

				_cancel_completion();
				completion_hint = "";

			} break;
			case KEY_KP_3: {
				if (k->get_unicode() != 0) {
					scancode_handled = false;
					break;
				}
				FALLTHROUGH;
			}
			case KEY_PAGEDOWN: {
				if (k->get_shift()) {
					_pre_shift_selection();
				}

				int wi;
				int n_line = cursor.line + num_lines_from_rows(cursor.line, get_cursor_wrap_index(), get_visible_rows(), wi) - 1;
				cursor_set_line(n_line, true, false, wi);

				if (k->get_shift()) {
					_post_shift_selection();
				}

				_cancel_completion();
				completion_hint = "";

			} break;
			case KEY_A: {
#ifndef APPLE_STYLE_KEYS
				if (!k->get_control() || k->get_shift() || k->get_alt()) {
					scancode_handled = false;
					break;
				}
				if (is_shortcut_keys_enabled()) {
					select_all();
				}
#else
				if ((!k->get_command() && !k->get_control())) {
					scancode_handled = false;
					break;
				}
				if (!k->get_shift() && k->get_command() && is_shortcut_keys_enabled())
					select_all();
				else if (k->get_control()) {
					if (k->get_shift())
						_pre_shift_selection();

					int current_line_whitespace_len = 0;
					while (current_line_whitespace_len < text[cursor.line].length()) {
						CharType c = text[cursor.line][current_line_whitespace_len];
						if (c != '\t' && c != ' ')
							break;
						current_line_whitespace_len++;
					}

					if (cursor_get_column() == current_line_whitespace_len)
						cursor_set_column(0);
					else
						cursor_set_column(current_line_whitespace_len);

					if (k->get_shift())
						_post_shift_selection();
					else if (k->get_command() || k->get_control())
						deselect();
				}
			} break;
			case KEY_E: {
				if (!k->get_control() || k->get_command() || k->get_alt()) {
					scancode_handled = false;
					break;
				}

				if (k->get_shift())
					_pre_shift_selection();

				if (k->get_command())
					cursor_set_line(text.size() - 1, true, false);
				cursor_set_column(text[cursor.line].length());

				if (k->get_shift())
					_post_shift_selection();
				else if (k->get_command() || k->get_control())
					deselect();

				_cancel_completion();
				completion_hint = "";
#endif
			} break;
			case KEY_X: {
				if (readonly) {
					break;
				}
				if (!k->get_command() || k->get_shift() || k->get_alt()) {
					scancode_handled = false;
					break;
				}
				if (is_shortcut_keys_enabled()) {
					cut();
				}

			} break;
			case KEY_C: {
				if (!k->get_command() || k->get_shift() || k->get_alt()) {
					scancode_handled = false;
					break;
				}

				if (is_shortcut_keys_enabled()) {
					copy();
				}

			} break;
			case KEY_Z: {
				if (readonly) {
					break;
				}

				if (!k->get_command()) {
					scancode_handled = false;
					break;
				}

				if (is_shortcut_keys_enabled()) {
					if (k->get_shift()) {
						redo();
					} else {
						undo();
					}
				}
			} break;
			case KEY_Y: {
				if (readonly) {
					break;
				}

				if (!k->get_command()) {
					scancode_handled = false;
					break;
				}

				if (is_shortcut_keys_enabled()) {
					redo();
				}
			} break;
			case KEY_V: {
				if (readonly) {
					break;
				}
				if (!k->get_command() || k->get_shift() || k->get_alt()) {
					scancode_handled = false;
					break;
				}

				if (is_shortcut_keys_enabled()) {
					paste();
				}

			} break;
			case KEY_SPACE: {
#ifdef OSX_ENABLED
				if (k->get_metakey()) { // cmd-space is spotlight shortcut in OSX
#else
				if (k->get_command()) {
#endif
					/* GCL: Ctrl+Space HER ZAMAN tamamlama istegi atar.
					   Eski kosul 'completion_enabled' bayragini kontrol ediyordu;
					   o bayrak henuz set edilmemisse Ctrl+Space sessizce
					   yutuluyordu - yazarken popup aciliyor ama kisa yol
					   hicbir sey yapmiyordu. Manuel (forced) tamamlama bu
					   bayraktan bagimsizdir. */
					query_code_comple();
					scancode_handled = true;
				} else {
					scancode_handled = false;
				}

			} break;

			case KEY_MENU: {
				if (context_menu_enabled) {
					popup_show = true;
					if (!readonly) {
						menu->set_item_disabled(menu->get_item_index(MENU_UNDO), !has_undo());
						menu->set_item_disabled(menu->get_item_index(MENU_REDO), !has_redo());
					}

					menu->set_position(get_global_transform().xform(_get_cursor_pixel_pos()));
					menu->set_size(Vector2(1, 1));
					menu->set_scale(get_global_transform().get_scale());
					menu->popup();
					menu->grab_focus();
				}
			} break;

			default: {
				scancode_handled = false;
			} break;
		}

		if (scancode_handled) {
			accept_event();
		}

		if (k->get_scancode() == KEY_INSERT) {
			set_insert_mode(!insert_mode);
			accept_event();
			return;
		}

		if (!scancode_handled && !k->get_command()) { // For German keyboards.

			if (k->get_unicode() >= 32) {
				if (readonly) {
					return;
				}

				// Remove the old character if in insert mode and no selection.
				if (insert_mode && !had_selection) {
					begin_complex_operation();

					// Make sure we don't try and remove empty space.
					if (cursor.column < get_line(cursor.line).length()) {
						_remove_text(cursor.line, cursor.column, cursor.line, cursor.column + 1);
					}
				}

				const CharType chr[2] = { (CharType)k->get_unicode(), 0 };

				if (completion_hint != "" && k->get_unicode() == ')') {
					completion_hint = "";
				}
				if (auto_brace_completion_enabled && _is_pair_symbol(chr[0])) {
					_consume_pair_symbol(chr[0]);
				} else {
					_insert_text_at_cursor(chr);
				}

				if (insert_mode && !had_selection) {
					end_complex_operation();
				}

				if (selection.active != had_selection) {
					end_complex_operation();
				}
				accept_event();
			}
		}

		return;
	}
}
