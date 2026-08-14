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

void TextEdit::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_caches();
			if (cursor_changed_dirty) {
				MessageQueue::get_singleton()->push_call(this, "_cursor_changed_emit");
			}
			if (text_changed_dirty) {
				MessageQueue::get_singleton()->push_call(this, "_text_changed_emit");
			}
			_update_wrap_at();
		} break;
		case NOTIFICATION_RESIZED: {
			_update_scrollbars();
			_update_wrap_at();
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible()) {
				call_deferred("_update_scrollbars");
				call_deferred("_update_wrap_at");
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_caches();
			_update_wrap_at();
			syntax_highlighting_cache.clear();
		} break;
		case MainLoop::NOTIFICATION_WM_FOCUS_IN: {
			window_has_focus = true;
			draw_caret = true;
			update();
		} break;
		case MainLoop::NOTIFICATION_WM_FOCUS_OUT: {
			window_has_focus = false;
			draw_caret = false;
			update();
		} break;
		case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			if (scrolling && get_v_scroll() != target_v_scroll) {
				double target_y = target_v_scroll - get_v_scroll();
				double dist = sqrt(target_y * target_y);
				// To ensure minimap is responsive override the speed setting.
				double vel = ((target_y / dist) * ((minimap_clicked) ? 3000 : v_scroll_speed)) * get_physics_process_delta_time();

				if (Math::abs(vel) >= dist) {
					set_v_scroll(target_v_scroll);
					scrolling = false;
					minimap_clicked = false;
					set_physics_process_internal(false);
				} else {
					set_v_scroll(get_v_scroll() + vel);
				}
			} else {
				scrolling = false;
				minimap_clicked = false;
				set_physics_process_internal(false);
			}
		} break;
		case NOTIFICATION_DRAW: {
			if (first_draw) {
				// Size may not be the final one, so attempts to ensure cursor was visible may have failed.
				adjust_viewport_to_cursor();
				first_draw = false;
			}
			Size2 size = get_size();
			if ((!has_focus() && !menu->has_focus()) || !window_has_focus) {
				draw_caret = false;
			}

			if (draw_breakpoint_gutter || draw_bookmark_gutter) {
				breakpoint_gutter_width = (get_row_height() * 55) / 100;
				cache.breakpoint_gutter_width = breakpoint_gutter_width;
			} else {
				cache.breakpoint_gutter_width = 0;
			}

			if (draw_info_gutter) {
				info_gutter_width = (get_row_height());
				cache.info_gutter_width = info_gutter_width;
			} else {
				cache.info_gutter_width = 0;
			}

			if (draw_fold_gutter) {
				fold_gutter_width = (get_row_height() * 55) / 100;
				cache.fold_gutter_width = fold_gutter_width;
			} else {
				cache.fold_gutter_width = 0;
			}

			cache.minimap_width = 0;
			if (draw_minimap) {
				cache.minimap_width = minimap_width;
			}

			int line_number_char_count = 0;

			{
				int lc = text.size();
				cache.line_number_w = 0;
				while (lc) {
					cache.line_number_w += 1;
					lc /= 10;
				};

				if (line_numbers) {
					line_number_char_count = cache.line_number_w;
					cache.line_number_w = (cache.line_number_w + 1) * cache.font->get_char_size('0').width;
				} else {
					cache.line_number_w = 0;
				}
			}
			_update_scrollbars();

			RID ci = get_canvas_item();
			VisualServer::get_singleton()->canvas_item_set_clip(get_canvas_item(), true);
			int xmargin_beg = cache.style_normal->get_margin(MARGIN_LEFT) + cache.line_number_w + cache.breakpoint_gutter_width + cache.fold_gutter_width + cache.info_gutter_width;

			int xmargin_end = size.width - cache.style_normal->get_margin(MARGIN_RIGHT) - cache.minimap_width;
			// Let's do it easy for now.
			cache.style_normal->draw(ci, Rect2(Point2(), size));
			if (readonly) {
				cache.style_readonly->draw(ci, Rect2(Point2(), size));
				draw_caret = false;
			}
			if (has_focus()) {
				cache.style_focus->draw(ci, Rect2(Point2(), size));
			}

			int ascent = cache.font->get_ascent();

			int visible_rows = get_visible_rows() + 1;

			Color color = readonly ? cache.font_color_readonly : cache.font_color;

			if (syntax_coloring) {
				if (cache.background_color.a > 0.01) {
					VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(), get_size()), cache.background_color);
				}
			}

			if (line_length_guidelines) {
				const int hard_x = xmargin_beg + (int)cache.font->get_char_size('0').width * line_length_guideline_hard_col - cursor.x_ofs;
				if (hard_x > xmargin_beg && hard_x < xmargin_end) {
					VisualServer::get_singleton()->canvas_item_add_line(ci, Point2(hard_x, 0), Point2(hard_x, size.height), cache.line_length_guideline_color);
				}

				// Draw a "Soft" line length guideline, less visible than the hard line length guideline.
				// It's usually set to a lower column compared to the hard line length guideline.
				// Only drawn if its column differs from the hard line length guideline.
				const int soft_x = xmargin_beg + (int)cache.font->get_char_size('0').width * line_length_guideline_soft_col - cursor.x_ofs;
				if (hard_x != soft_x && soft_x > xmargin_beg && soft_x < xmargin_end) {
					VisualServer::get_singleton()->canvas_item_add_line(ci, Point2(soft_x, 0), Point2(soft_x, size.height), cache.line_length_guideline_color * Color(1, 1, 1, 0.5));
				}
			}

			int brace_open_match_line = -1;
			int brace_open_match_column = -1;
			bool brace_open_matching = false;
			bool brace_open_mismatch = false;
			int brace_close_match_line = -1;
			int brace_close_match_column = -1;
			bool brace_close_matching = false;
			bool brace_close_mismatch = false;

			if (brace_matching_enabled && cursor.line >= 0 && cursor.line < text.size() && cursor.column >= 0) {
				if (cursor.column < text[cursor.line].length()) {
					// Check for open.
					CharType c = text[cursor.line][cursor.column];
					CharType closec = 0;

					if (c == '[') {
						closec = ']';
					} else if (c == '{') {
						closec = '}';
					} else if (c == '(') {
						closec = ')';
					}

					if (closec != 0) {
						int stack = 1;

						for (int i = cursor.line; i < text.size(); i++) {
							int from = i == cursor.line ? cursor.column + 1 : 0;
							for (int j = from; j < text[i].length(); j++) {
								CharType cc = text[i][j];
								// Ignore any brackets inside a string.
								if (cc == '"' || cc == '\'') {
									CharType quotation = cc;
									do {
										j++;
										if (!(j < text[i].length())) {
											break;
										}
										cc = text[i][j];
										// Skip over escaped quotation marks inside strings.
										if (cc == '\\') {
											bool escaped = true;
											while (j + 1 < text[i].length() && text[i][j + 1] == '\\') {
												escaped = !escaped;
												j++;
											}
											if (escaped) {
												j++;
												continue;
											}
										}
									} while (cc != quotation);
								} else if (cc == c) {
									stack++;
								} else if (cc == closec) {
									stack--;
								}

								if (stack == 0) {
									brace_open_match_line = i;
									brace_open_match_column = j;
									brace_open_matching = true;

									break;
								}
							}
							if (brace_open_match_line != -1) {
								break;
							}
						}

						if (!brace_open_matching) {
							brace_open_mismatch = true;
						}
					}
				}

				if (cursor.column > 0) {
					CharType c = text[cursor.line][cursor.column - 1];
					CharType closec = 0;

					if (c == ']') {
						closec = '[';
					} else if (c == '}') {
						closec = '{';
					} else if (c == ')') {
						closec = '(';
					}

					if (closec != 0) {
						int stack = 1;

						for (int i = cursor.line; i >= 0; i--) {
							int from = i == cursor.line ? cursor.column - 2 : text[i].length() - 1;
							for (int j = from; j >= 0; j--) {
								CharType cc = text[i][j];
								// Ignore any brackets inside a string.
								if (cc == '"' || cc == '\'') {
									CharType quotation = cc;
									do {
										j--;
										if (!(j >= 0)) {
											break;
										}
										cc = text[i][j];
										// Skip over escaped quotation marks inside strings.
										if (cc == quotation) {
											bool escaped = false;
											while (j - 1 >= 0 && text[i][j - 1] == '\\') {
												escaped = !escaped;
												j--;
											}
											if (escaped) {
												cc = '\\';
												continue;
											}
										}
									} while (cc != quotation);
								} else if (cc == c) {
									stack++;
								} else if (cc == closec) {
									stack--;
								}

								if (stack == 0) {
									brace_close_match_line = i;
									brace_close_match_column = j;
									brace_close_matching = true;

									break;
								}
							}
							if (brace_close_match_line != -1) {
								break;
							}
						}

						if (!brace_close_matching) {
							brace_close_mismatch = true;
						}
					}
				}
			}

			Point2 cursor_pos;
			bool is_cursor_visible = false;
			int cursor_insert_offset_y = 0;

			// Get the highlighted words.
			String highlighted_text = get_selection_text();

			// Check if highlighted words contains only whitespaces (tabs or spaces).
			bool only_whitespaces_highlighted = highlighted_text.strip_edges() == String();

			String line_num_padding = line_numbers_zero_padded ? "0" : " ";

			int cursor_wrap_index = get_cursor_wrap_index();

			FontDrawer drawer(cache.font, Color(1, 1, 1));

			int first_visible_line = get_first_visible_line() - 1;
			int draw_amount = visible_rows + (smooth_scroll_enabled ? 1 : 0);
			draw_amount += times_line_wraps(first_visible_line + 1);

			// Draw minimap.
			if (draw_minimap) {
				int minimap_visible_lines = _get_minimap_visible_rows();
				int minimap_line_height = (minimap_char_size.y + minimap_line_spacing);
				int minimap_tab_size = minimap_char_size.x * indent_size;

				// calculate viewport size and y offset
				int viewport_height = (draw_amount - 1) * minimap_line_height;
				int control_height = _get_control_height() - viewport_height;
				int viewport_offset_y = round(get_scroll_pos_for_line(first_visible_line + 1) * control_height) / ((v_scroll->get_max() <= minimap_visible_lines) ? (minimap_visible_lines - draw_amount) : (v_scroll->get_max() - draw_amount));

				// calculate the first line.
				int num_lines_before = round((viewport_offset_y) / minimap_line_height);
				int wi;
				int minimap_line = (v_scroll->get_max() <= minimap_visible_lines) ? -1 : first_visible_line;
				if (minimap_line >= 0) {
					minimap_line -= num_lines_from_rows(first_visible_line, 0, -num_lines_before, wi);
					minimap_line -= (minimap_line > 0 && smooth_scroll_enabled ? 1 : 0);
				}
				int minimap_draw_amount = minimap_visible_lines + times_line_wraps(minimap_line + 1);

				// Draw the minimap.

				// Add visual feedback when dragging or hovering the the visible area rectangle.
				float viewport_alpha;
				if (dragging_minimap) {
					viewport_alpha = 0.25;
				} else if (hovering_minimap) {
					viewport_alpha = 0.175;
				} else {
					viewport_alpha = 0.1;
				}

				const Color viewport_color = (cache.background_color.get_v() < 0.5) ? Color(1, 1, 1, viewport_alpha) : Color(0, 0, 0, viewport_alpha);
				VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2((xmargin_end + 2), viewport_offset_y, cache.minimap_width, viewport_height), viewport_color);
				for (int i = 0; i < minimap_draw_amount; i++) {
					minimap_line++;

					if (minimap_line < 0 || minimap_line >= (int)text.size()) {
						break;
					}

					while (is_line_hidden(minimap_line)) {
						minimap_line++;
						if (minimap_line < 0 || minimap_line >= (int)text.size()) {
							break;
						}
					}

					if (minimap_line < 0 || minimap_line >= (int)text.size()) {
						break;
					}

					Map<int, HighlighterInfo> color_map;
					if (syntax_coloring) {
						color_map = _get_line_syntax_highlighting(minimap_line);
					}

					Color current_color = cache.font_color;
					if (readonly) {
						current_color = cache.font_color_readonly;
					}

					Vector<String> wrap_rows = get_wrap_rows_text(minimap_line);
					int line_wrap_amount = times_line_wraps(minimap_line);
					int last_wrap_column = 0;

					for (int line_wrap_index = 0; line_wrap_index < line_wrap_amount + 1; line_wrap_index++) {
						if (line_wrap_index != 0) {
							i++;
							if (i >= minimap_draw_amount) {
								break;
							}
						}

						const String &str = wrap_rows[line_wrap_index];
						int indent_px = line_wrap_index != 0 ? get_indent_level(minimap_line) : 0;
						if (indent_px >= wrap_at) {
							indent_px = 0;
						}
						indent_px = minimap_char_size.x * indent_px;

						if (line_wrap_index > 0) {
							last_wrap_column += wrap_rows[line_wrap_index - 1].length();
						}

						if (minimap_line == cursor.line && cursor_wrap_index == line_wrap_index && highlight_current_line) {
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2((xmargin_end + 2), i * 3, cache.minimap_width, 2), cache.current_line_color);
						}

						Color previous_color;
						int characters = 0;
						int tabs = 0;
						for (int j = 0; j < str.length(); j++) {
							if (syntax_coloring) {
								if (color_map.has(last_wrap_column + j)) {
									current_color = color_map[last_wrap_column + j].color;
									if (readonly) {
										current_color.a = cache.font_color_readonly.a;
									}
								}
								color = current_color;
							}

							if (j == 0) {
								previous_color = color;
							}

							int xpos = indent_px + ((xmargin_end + minimap_char_size.x) + (minimap_char_size.x * j)) + tabs;
							bool out_of_bounds = (xpos >= xmargin_end + cache.minimap_width);

							bool is_whitespace = _is_whitespace(str[j]);
							if (!is_whitespace) {
								characters++;

								if (j < str.length() - 1 && color == previous_color && !out_of_bounds) {
									continue;
								}

								// If we've changed colour we are at the start of a new section, therefore we need to go back to the end
								// of the previous section to draw it, we'll also add the character back on.
								if (color != previous_color) {
									characters--;
									j--;

									if (str[j] == '\t') {
										tabs -= minimap_tab_size;
									}
								}
							}

							if (characters > 0) {
								previous_color.a *= 0.6;
								// take one for zero indexing, and if we hit whitespace / the end of a word.
								int chars = MAX(0, (j - (characters - 1)) - (is_whitespace ? 1 : 0)) + 1;
								int char_x_ofs = indent_px + ((xmargin_end + minimap_char_size.x) + (minimap_char_size.x * chars)) + tabs;
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2(char_x_ofs, minimap_line_height * i), Point2(minimap_char_size.x * characters, minimap_char_size.y)), previous_color);
							}

							if (out_of_bounds) {
								break;
							}

							// re-adjust if we went backwards.
							if (color != previous_color && !is_whitespace) {
								characters++;
							}

							if (str[j] == '\t') {
								tabs += minimap_tab_size;
							}

							previous_color = color;
							characters = 0;
						}
					}
				}
			}

			int top_limit_y = 0;
			int bottom_limit_y = get_size().height;
			if (readonly) {
				top_limit_y += cache.style_readonly->get_margin(MARGIN_BOTTOM);
				bottom_limit_y -= cache.style_readonly->get_margin(MARGIN_BOTTOM);
			} else {
				top_limit_y += cache.style_normal->get_margin(MARGIN_TOP);
				bottom_limit_y -= cache.style_normal->get_margin(MARGIN_TOP);
			}

			// Draw main text.
			line_drawing_cache.clear();
			int line = first_visible_line;
			for (int i = 0; i < draw_amount; i++) {
				line++;

				if (line < 0 || line >= (int)text.size()) {
					continue;
				}

				while (is_line_hidden(line)) {
					line++;
					if (line < 0 || line >= (int)text.size()) {
						break;
					}
				}

				if (line < 0 || line >= (int)text.size()) {
					continue;
				}

				const String &fullstr = text[line];
				LineDrawingCache cache_entry;

				Map<int, HighlighterInfo> color_map;
				if (syntax_coloring) {
					color_map = _get_line_syntax_highlighting(line);
				}
				// Ensure we at least use the font color.
				Color current_color = readonly ? cache.font_color_readonly : cache.font_color;

				bool underlined = false;

				Vector<String> wrap_rows = get_wrap_rows_text(line);
				int line_wrap_amount = times_line_wraps(line);
				int last_wrap_column = 0;
				int wrap_column_offset = 0;

				for (int line_wrap_index = 0; line_wrap_index < line_wrap_amount + 1; line_wrap_index++) {
					if (line_wrap_index != 0) {
						i++;
						if (i >= draw_amount) {
							break;
						}
					}

					const String &str = wrap_rows[line_wrap_index];
					int indent_px = line_wrap_index != 0 ? get_indent_level(line) * cache.font->get_char_size(' ').width : 0;
					if (indent_px >= wrap_at) {
						indent_px = 0;
					}

					if (line_wrap_index > 0) {
						last_wrap_column += wrap_rows[line_wrap_index - 1].length();
					}

					int char_margin = xmargin_beg - cursor.x_ofs;
					char_margin += indent_px;
					int char_ofs = 0;

					int ofs_x = 0;
					int ofs_y = 0;
					if (readonly) {
						ofs_x = cache.style_readonly->get_offset().x / 2;
						ofs_x -= cache.style_normal->get_offset().x / 2;
						ofs_y = cache.style_readonly->get_offset().y / 2;
					} else {
						ofs_y = cache.style_normal->get_offset().y / 2;
					}

					ofs_y += (i * get_row_height() + cache.line_spacing / 2);
					ofs_y -= cursor.wrap_ofs * get_row_height();
					ofs_y -= get_v_scroll_offset() * get_row_height();

					bool clipped = false;
					if (ofs_y + get_row_height() < top_limit_y) {
						// Line is outside the top margin, clip current line.
						// Still need to go through the process to prepare color changes for next lines.
						clipped = true;
					}

					if (ofs_y > bottom_limit_y) {
						// Line is outside the bottom margin, clip any remaining text.
						i = draw_amount;
						break;
					}

					// Check if line contains highlighted word.
					int highlighted_text_col = -1;
					int search_text_col = -1;
					int highlighted_word_col = -1;

					if (!search_text.empty()) {
						search_text_col = _get_column_pos_of_word(search_text, str, search_flags, 0);
					}

					if (highlighted_text.length() != 0 && highlighted_text != search_text) {
						highlighted_text_col = _get_column_pos_of_word(highlighted_text, str, SEARCH_MATCH_CASE | SEARCH_WHOLE_WORDS, 0);
					}

					if (select_identifiers_enabled && highlighted_word.length() != 0) {
						if (_is_char(highlighted_word[0]) || highlighted_word[0] == '.') {
							highlighted_word_col = _get_column_pos_of_word(highlighted_word, fullstr, SEARCH_MATCH_CASE | SEARCH_WHOLE_WORDS, 0);
						}
					}

					if (text.is_marked(line)) {
						VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y, xmargin_end - xmargin_beg, get_row_height()), cache.mark_color);
					}

					if (str.length() == 0) {
						// Draw line background if empty as we won't loop at at all.
						if (line == cursor.line && cursor_wrap_index == line_wrap_index && highlight_current_line) {
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(ofs_x, ofs_y, xmargin_end, get_row_height()), cache.current_line_color);
						}

						// Give visual indication of empty selected line.
						if (selection.active && line >= selection.from_line && line <= selection.to_line && char_margin >= xmargin_beg) {
							int char_w = cache.font->get_char_size(' ').width;
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y, char_w, get_row_height()), cache.selection_color);
						}
					} else {
						// If it has text, then draw current line marker in the margin, as line number etc will draw over it, draw the rest of line marker later.
						if (line == cursor.line && cursor_wrap_index == line_wrap_index && highlight_current_line) {
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(0, ofs_y, xmargin_beg + ofs_x, get_row_height()), cache.current_line_color);
						}
					}

					if (line_wrap_index == 0) {
						// Only do these if we are on the first wrapped part of a line.

						cache_entry.y_offset = ofs_y;

						if (text.is_breakpoint(line) && !draw_breakpoint_gutter) {


#ifdef TOOLS_ENABLED
        VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y + get_row_height() - EDSCALE, xmargin_end - xmargin_beg, EDSCALE), cache.breakpoint_color);
#else
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y, xmargin_end - xmargin_beg, get_row_height()), cache.breakpoint_color);
#endif
						}

						// Draw bookmark marker.
						if (text.is_bookmark(line)) {
							if (draw_bookmark_gutter) {
								int vertical_gap = (get_row_height() * 40) / 100;
								int horizontal_gap = (cache.breakpoint_gutter_width * 30) / 100;
								int marker_radius = get_row_height() - (vertical_gap * 2);
								VisualServer::get_singleton()->canvas_item_add_circle(ci, Point2(cache.style_normal->get_margin(MARGIN_LEFT) + horizontal_gap - 2 + marker_radius / 2, ofs_y + vertical_gap + marker_radius / 2), marker_radius, Color(cache.bookmark_color.r, cache.bookmark_color.g, cache.bookmark_color.b));
							}
						}

						// Draw breakpoint marker.
						if (text.is_breakpoint(line)) {
							if (draw_breakpoint_gutter) {
								int vertical_gap = (get_row_height() * 40) / 100;
								int horizontal_gap = (cache.breakpoint_gutter_width * 30) / 100;
								int marker_height = get_row_height() - (vertical_gap * 2);
								int marker_width = cache.breakpoint_gutter_width - (horizontal_gap * 2);
								// No transparency on marker.
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(cache.style_normal->get_margin(MARGIN_LEFT) + horizontal_gap - 2, ofs_y + vertical_gap, marker_width, marker_height), Color(cache.breakpoint_color.r, cache.breakpoint_color.g, cache.breakpoint_color.b));
							}
						}

						// Draw info icons.
						if (draw_info_gutter && text.has_info_icon(line)) {
							int vertical_gap = (get_row_height() * 40) / 100;
							int horizontal_gap = (cache.info_gutter_width * 30) / 100;
							int gutter_left = cache.style_normal->get_margin(MARGIN_LEFT) + cache.breakpoint_gutter_width;

							Ref<Texture> info_icon = text.get_info_icon(line);
							// Ensure the icon fits the gutter size.
							Size2i icon_size = info_icon->get_size();
							if (icon_size.width > cache.info_gutter_width - horizontal_gap) {
								icon_size.width = cache.info_gutter_width - horizontal_gap;
							}
							if (icon_size.height > get_row_height() - horizontal_gap) {
								icon_size.height = get_row_height() - horizontal_gap;
							}

							Size2i icon_pos;
							int xofs = horizontal_gap - (info_icon->get_width() / 4);
							int yofs = vertical_gap - (info_icon->get_height() / 4);
							icon_pos.x = gutter_left + xofs + ofs_x;
							icon_pos.y = ofs_y + yofs;

							draw_texture_rect(info_icon, Rect2(icon_pos, icon_size));
						}

						// Draw execution marker.
						if (executing_line == line) {
							if (draw_breakpoint_gutter) {
								int icon_extra_size = 4;
								int vertical_gap = (get_row_height() * 40) / 100;
								int horizontal_gap = (cache.breakpoint_gutter_width * 30) / 100;
								int marker_height = get_row_height() - (vertical_gap * 2) + icon_extra_size;
								int marker_width = cache.breakpoint_gutter_width - (horizontal_gap * 2) + icon_extra_size;
								cache.executing_icon->draw_rect(ci, Rect2(cache.style_normal->get_margin(MARGIN_LEFT) + horizontal_gap - 2 - icon_extra_size / 2, ofs_y + vertical_gap - icon_extra_size / 2, marker_width, marker_height), false, Color(cache.executing_line_color.r, cache.executing_line_color.g, cache.executing_line_color.b));
							} else {
#ifdef TOOLS_ENABLED
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y + get_row_height() - EDSCALE, xmargin_end - xmargin_beg, EDSCALE), cache.executing_line_color);
#else
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y, xmargin_end - xmargin_beg, get_row_height()), cache.executing_line_color);
#endif
							}
						}

						// Draw fold markers.
						if (draw_fold_gutter) {
							int horizontal_gap = (cache.fold_gutter_width * 30) / 100;
							int gutter_left = cache.style_normal->get_margin(MARGIN_LEFT) + cache.breakpoint_gutter_width + cache.line_number_w + cache.info_gutter_width;
							if (is_folded(line)) {
								int xofs = horizontal_gap - (cache.can_fold_icon->get_width()) / 2;
								int yofs = (get_row_height() - cache.folded_icon->get_height()) / 2;
								cache.folded_icon->draw(ci, Point2(gutter_left + xofs + ofs_x, ofs_y + yofs), cache.code_folding_color);
							} else if (can_fold(line)) {
								int xofs = -cache.can_fold_icon->get_width() / 2 - horizontal_gap + 3;
								int yofs = (get_row_height() - cache.can_fold_icon->get_height()) / 2;
								cache.can_fold_icon->draw(ci, Point2(gutter_left + xofs + ofs_x, ofs_y + yofs), cache.code_folding_color);
							}
						}

						// Draw line numbers.
						if (cache.line_number_w) {
							int yofs = ofs_y + (get_row_height() - cache.font->get_height()) / 2;
							String fc = String::num(line + 1);
							while (fc.length() < line_number_char_count) {
								fc = line_num_padding + fc;
							}

							cache.font->draw(ci, Point2(cache.style_normal->get_margin(MARGIN_LEFT) + cache.breakpoint_gutter_width + cache.info_gutter_width + ofs_x, yofs + cache.font->get_ascent()), fc, text.is_safe(line) ? cache.safe_line_number_color : cache.line_number_color);
						}
					}

					int first_visible_char = str.length();
					int last_visible_char = 0;

					// Loop through characters in one line.
					int j = 0;
					for (; j < str.length(); j++) {
						if (syntax_coloring) {
							if (color_map.has(last_wrap_column + j)) {
								current_color = color_map[last_wrap_column + j].color;
								if (readonly && current_color.a > cache.font_color_readonly.a) {
									current_color.a = cache.font_color_readonly.a;
								}
							}
							color = current_color;
						}

						int char_w;

						// Handle tabulator.
						char_w = text.get_char_width(str[j], str[j + 1], char_ofs);

						if ((char_ofs + char_margin) < xmargin_beg) {
							char_ofs += char_w;

							// Line highlighting handle horizontal clipping.
							if (line == cursor.line && cursor_wrap_index == line_wrap_index && highlight_current_line) {
								if (j == str.length() - 1) {
									// End of line when last char is skipped.
									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y, xmargin_end - (char_ofs + char_margin + char_w), get_row_height()), cache.current_line_color);
								} else if ((char_ofs + char_margin) > xmargin_beg) {
									// Char next to margin is skipped.
									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(xmargin_beg + ofs_x, ofs_y, (char_ofs + char_margin) - (xmargin_beg + ofs_x), get_row_height()), cache.current_line_color);
								}
							}
							continue;
						}

						if ((char_ofs + char_margin + char_w) >= xmargin_end) {
							break;
						}

						bool in_search_result = false;

						if (search_text_col != -1) {
							// If we are at the end check for new search result on same line.
							if (j >= search_text_col + search_text.length()) {
								search_text_col = _get_column_pos_of_word(search_text, str, search_flags, j);
							}

							in_search_result = j >= search_text_col && j < search_text_col + search_text.length();

							if (in_search_result) {
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin, ofs_y), Size2i(char_w, get_row_height())), cache.search_result_color);
							}
						}

						// Current line highlighting.
						bool in_selection = (selection.active && line >= selection.from_line && line <= selection.to_line && (line > selection.from_line || last_wrap_column + j >= selection.from_column) && (line < selection.to_line || last_wrap_column + j < selection.to_column));

						if (!clipped && line == cursor.line && cursor_wrap_index == line_wrap_index && highlight_current_line) {
							// Draw the wrap indent offset highlight.
							if (line_wrap_index != 0 && j == 0) {
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(char_ofs + char_margin + ofs_x - indent_px, ofs_y, indent_px, get_row_height()), cache.current_line_color);
							}
							// If its the last char draw to end of the line.
							if (j == str.length() - 1) {
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(char_ofs + char_margin + char_w + ofs_x, ofs_y, xmargin_end - (char_ofs + char_margin + char_w), get_row_height()), cache.current_line_color);
							}
							// Actual text.
							if (!in_selection) {
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + ofs_x, ofs_y), Size2i(char_w, get_row_height())), cache.current_line_color);
							}
						}

						if (!clipped && in_selection) {
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + ofs_x, ofs_y), Size2i(char_w, get_row_height())), cache.selection_color);
						}

						if (!clipped && in_search_result) {
							Color border_color = (line == search_result_line && j >= search_result_col && j < search_result_col + search_text.length()) ? cache.font_color : cache.search_result_border_color;

							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + ofs_x, ofs_y), Size2i(char_w, 1)), border_color);
							VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + ofs_x, ofs_y + get_row_height() - 1), Size2i(char_w, 1)), border_color);

							if (j == search_text_col) {
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + ofs_x, ofs_y), Size2i(1, get_row_height())), border_color);
							}
							if (j == search_text_col + search_text.length() - 1) {
								VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + char_w + ofs_x - 1, ofs_y), Size2i(1, get_row_height())), border_color);
							}
						}

						if (!clipped && highlight_all_occurrences && !only_whitespaces_highlighted) {
							if (highlighted_text_col != -1) {
								// If we are at the end check for new word on same line.
								if (j > highlighted_text_col + highlighted_text.length()) {
									highlighted_text_col = _get_column_pos_of_word(highlighted_text, str, SEARCH_MATCH_CASE | SEARCH_WHOLE_WORDS, j);
								}

								bool in_highlighted_word = (j >= highlighted_text_col && j < highlighted_text_col + highlighted_text.length());

								// If this is the original highlighted text we don't want to highlight it again.
								if (cursor.line == line && cursor_wrap_index == line_wrap_index && (cursor.column >= highlighted_text_col && cursor.column <= highlighted_text_col + highlighted_text.length())) {
									in_highlighted_word = false;
								}

								if (in_highlighted_word) {
									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2i(char_ofs + char_margin + ofs_x, ofs_y), Size2i(char_w, get_row_height())), cache.word_highlighted_color);
								}
							}
						}

						if (highlighted_word_col != -1) {
							if (j + last_wrap_column > highlighted_word_col + highlighted_word.length()) {
								highlighted_word_col = _get_column_pos_of_word(highlighted_word, fullstr, SEARCH_MATCH_CASE | SEARCH_WHOLE_WORDS, j + last_wrap_column);
							}
							underlined = (j + last_wrap_column >= highlighted_word_col && j + last_wrap_column < highlighted_word_col + highlighted_word.length());
						}

						if (brace_matching_enabled) {
							int yofs = ofs_y + (get_row_height() - cache.font->get_height()) / 2;
							if ((brace_open_match_line == line && brace_open_match_column == last_wrap_column + j) ||
									(cursor.column == last_wrap_column + j && cursor.line == line && cursor_wrap_index == line_wrap_index && (brace_open_matching || brace_open_mismatch))) {
								if (brace_open_mismatch) {
									color = cache.brace_mismatch_color;
								}
								drawer.draw_char(ci, Point2i(char_ofs + char_margin + ofs_x, yofs + ascent), '_', str[j + 1], in_selection && override_selected_font_color ? cache.font_color_selected : color);
							}

							if ((brace_close_match_line == line && brace_close_match_column == last_wrap_column + j) ||
									(cursor.column == last_wrap_column + j + 1 && cursor.line == line && cursor_wrap_index == line_wrap_index && (brace_close_matching || brace_close_mismatch))) {
								if (brace_close_mismatch) {
									color = cache.brace_mismatch_color;
								}
								drawer.draw_char(ci, Point2i(char_ofs + char_margin + ofs_x, yofs + ascent), '_', str[j + 1], in_selection && override_selected_font_color ? cache.font_color_selected : color);
							}
						}

						if (!clipped && cursor.column == last_wrap_column + j && cursor.line == line && cursor_wrap_index == line_wrap_index) {
							is_cursor_visible = true;
							cursor_pos = Point2i(char_ofs + char_margin + ofs_x, ofs_y);
							cursor_pos.y += (get_row_height() - cache.font->get_height()) / 2;

							if (insert_mode) {
								cursor_insert_offset_y = (cache.font->get_height() - 3);
								cursor_pos.y += cursor_insert_offset_y;
							}

							int caret_w = (str[j] == '\t') ? cache.font->get_char_size(' ').width : char_w;
							if (ime_text.length() > 0) {
								int ofs = 0;
								while (true) {
									if (ofs >= ime_text.length()) {
										break;
									}

									CharType cchar = ime_text[ofs];
									CharType next = ime_text[ofs + 1];
									int im_char_width = cache.font->get_char_size(cchar, next).width;

									if ((char_ofs + char_margin + im_char_width) >= xmargin_end) {
										break;
									}

									bool selected = ofs >= ime_selection.x && ofs < ime_selection.x + ime_selection.y;
									if (selected) {
										VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2(char_ofs + char_margin, ofs_y + get_row_height()), Size2(im_char_width, 3)), color);
									} else {
										VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2(char_ofs + char_margin, ofs_y + get_row_height()), Size2(im_char_width, 1)), color);
									}

									drawer.draw_char(ci, Point2(char_ofs + char_margin + ofs_x, ofs_y + ascent), cchar, next, color);

									char_ofs += im_char_width;
									ofs++;
								}
							}
							if (ime_text.length() == 0) {
								if (draw_caret || drag_caret_force_displayed) {
									if (insert_mode) {
#ifdef TOOLS_ENABLED
										int caret_h = (block_caret) ? 4 : 2 * EDSCALE;
#else
										int caret_h = (block_caret) ? 4 : 2;
#endif
										VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(cursor_pos, Size2i(caret_w, caret_h)), cache.caret_color);
									} else {
#ifdef TOOLS_ENABLED
										caret_w = (block_caret) ? caret_w : 2 * EDSCALE;
#else
										caret_w = (block_caret) ? caret_w : 2;
#endif

										VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(cursor_pos, Size2i(caret_w, cache.font->get_height())), cache.caret_color);
									}
								}
							}
						}

						if (!clipped) {
							if (cursor.column == last_wrap_column + j && cursor.line == line && cursor_wrap_index == line_wrap_index && block_caret && draw_caret && !insert_mode) {
								color = cache.caret_background_color;
							} else if (!syntax_coloring && block_caret) {
								color = readonly ? cache.font_color_readonly : cache.font_color;
							}

							if (str[j] >= 32) {
								int yofs = ofs_y + (get_row_height() - cache.font->get_height()) / 2;
								int w = drawer.draw_char(ci, Point2i(char_ofs + char_margin + ofs_x, yofs + ascent), str[j], str[j + 1], in_selection && override_selected_font_color ? cache.font_color_selected : color);
								if (underlined) {
									float line_width = 1.0;
#ifdef TOOLS_ENABLED
									line_width *= EDSCALE;
#endif

									draw_rect(Rect2(char_ofs + char_margin + ofs_x, yofs + ascent + 2, w, line_width), in_selection && override_selected_font_color ? cache.font_color_selected : color);
								}
							} else if (draw_tabs && str[j] == '\t') {
								int yofs = (get_row_height() - cache.tab_icon->get_height()) / 2;
								cache.tab_icon->draw(ci, Point2(char_ofs + char_margin + ofs_x, ofs_y + yofs), in_selection && override_selected_font_color ? cache.font_color_selected : color);
							}

							if (draw_spaces && str[j] == ' ') {
								int yofs = (get_row_height() - cache.space_icon->get_height()) / 2;
								cache.space_icon->draw(ci, Point2(char_ofs + char_margin + ofs_x, ofs_y + yofs), in_selection && override_selected_font_color ? cache.font_color_selected : color);
							}

							if (first_visible_char > j) {
								first_visible_char = j;
							}
							if (last_visible_char < j) {
								last_visible_char = j;
							}
						}

						char_ofs += char_w;

						if (line_wrap_index == line_wrap_amount && j == str.length() - 1 && is_folded(line)) {
							int yofs = (get_row_height() - cache.folded_eol_icon->get_height()) / 2;
							int xofs = cache.folded_eol_icon->get_width() / 2;
							Color eol_color = cache.code_folding_color;
							eol_color.a = 1;
							cache.folded_eol_icon->draw(ci, Point2(char_ofs + char_margin + xofs + ofs_x, ofs_y + yofs), eol_color);
						}
					}

					if (!clipped && cursor.column == (last_wrap_column + j) && cursor.line == line && cursor_wrap_index == line_wrap_index && (char_ofs + char_margin) >= xmargin_beg) {
						is_cursor_visible = true;
						cursor_pos = Point2i(char_ofs + char_margin + ofs_x, ofs_y);
						cursor_pos.y += (get_row_height() - cache.font->get_height()) / 2;

						if (insert_mode) {
							cursor_insert_offset_y = cache.font->get_height() - 3;
							cursor_pos.y += cursor_insert_offset_y;
						}
						if (ime_text.length() > 0) {
							int ofs = 0;
							while (true) {
								if (ofs >= ime_text.length()) {
									break;
								}

								CharType cchar = ime_text[ofs];
								CharType next = ime_text[ofs + 1];
								int im_char_width = cache.font->get_char_size(cchar, next).width;

								if ((char_ofs + char_margin + im_char_width) >= xmargin_end) {
									break;
								}

								bool selected = ofs >= ime_selection.x && ofs < ime_selection.x + ime_selection.y;
								if (selected) {
									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2(char_ofs + char_margin, ofs_y + get_row_height()), Size2(im_char_width, 3)), color);
								} else {
									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2(char_ofs + char_margin, ofs_y + get_row_height()), Size2(im_char_width, 1)), color);
								}

								drawer.draw_char(ci, Point2(char_ofs + char_margin + ofs_x, ofs_y + ascent), cchar, next, color);

								char_ofs += im_char_width;
								ofs++;
							}
						}
						if (ime_text.length() == 0) {
							if (draw_caret || drag_caret_force_displayed) {
								if (insert_mode) {
									int char_w = cache.font->get_char_size(' ').width;
#ifdef TOOLS_ENABLED
									int caret_h = (block_caret) ? 4 : 2 * EDSCALE;
#else
									int caret_h = (block_caret) ? 4 : 2;
#endif
									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(cursor_pos, Size2i(char_w, caret_h)), cache.caret_color);
								} else {
									int char_w = cache.font->get_char_size(' ').width;
#ifdef TOOLS_ENABLED
									int caret_w = (block_caret) ? char_w : 2 * EDSCALE;
#else
									int caret_w = (block_caret) ? char_w : 2;
#endif

									VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(cursor_pos, Size2i(caret_w, cache.font->get_height())), cache.caret_color);
								}
							}
						}
					}

					cache_entry.first_visible_char.push_back(wrap_column_offset + first_visible_char);
					cache_entry.last_visible_char.push_back(wrap_column_offset + last_visible_char);

					wrap_column_offset += str.length();
				}

				line_drawing_cache[line] = cache_entry;
			}

			// Flush out any text in the drawer BEFORE
			// drawing the completion box, as we want the completion
			// box to overwrite the underlying text.
			drawer.flush();

			bool completion_below = false;
			if (completion_active && is_cursor_visible && completion_options.size() > 0) {
				// Completion panel

				const Ref<StyleBox> csb = get_stylebox("completion");
				const int maxlines = get_constant("completion_lines");
				const int cmax_width = get_constant("completion_max_width") * cache.font->get_char_size('x').x;
				const Color scrollc = get_color("completion_scroll_color");

				const int row_height = get_row_height();
				const int completion_options_size = completion_options.size();
				const int row_count = MIN(completion_options_size, maxlines);
				const int completion_rows_height = row_count * row_height;
				const int completion_base_width = cache.font->get_string_size(completion_base).width;

				int scroll_rectangle_width = get_constant("completion_scroll_width");
				int width = 0;

				// Compute max width of the panel based on the longest completion option.
				// Limit the number of results for automatic width calculation to avoid freezing while showing results.
				if (completion_options_size < 1000) {
					for (int i = 0; i < completion_options_size; i++) {
						int line_width = MIN(cache.font->get_string_size(completion_options[i].display).x, cmax_width);
						if (line_width > width) {
							width = line_width;
						}
					}
				} else {
					// Fall back to predetermined width.
					width = cmax_width;
				}

				// Add space for completion icons.
				const int icon_hsep = get_constant("hseparation", "ItemList");
				const Size2 icon_area_size(row_height, row_height);
				const int icon_area_width = icon_area_size.width + icon_hsep;
				width += icon_area_size.width + icon_hsep;

				const int line_from = CLAMP((completion_force_item_center < 0 ? completion_index : completion_force_item_center) - row_count / 2, 0, completion_options_size - row_count);

				for (int i = 0; i < row_count; i++) {
					int l = line_from + i;
					ERR_CONTINUE(l < 0 || l >= completion_options_size);
					if (completion_options[l].default_value.get_type() == Variant::COLOR) {
						width += icon_area_size.width;
						break;
					}
				}

				// Position completion panel
				completion_rect.size.width = width + 2;
				completion_rect.size.height = completion_rows_height;

				if (completion_options_size <= maxlines) {
					scroll_rectangle_width = 0;
				}

				const Point2 csb_offset = csb->get_offset();
				const int total_height = completion_rect.size.height + csb->get_minimum_size().y;
				const int ajdusted_cursor_y = cursor_pos.y - cursor_insert_offset_y - (get_row_height() - cache.font->get_height()) / 2;

				completion_rect.position.x = cursor_pos.x - completion_base_width - icon_area_width - csb_offset.x;

				if (ajdusted_cursor_y + row_height + total_height > get_size().height && ajdusted_cursor_y > total_height) {
					// Completion panel above the cursor line
					completion_rect.position.y = ajdusted_cursor_y - total_height;
				} else {
					// Completion panel below the cursor line
					completion_rect.position.y = ajdusted_cursor_y + row_height;
					completion_below = true;
				}

				draw_style_box(csb, Rect2(completion_rect.position - csb_offset, completion_rect.size + csb->get_minimum_size() + Size2(scroll_rectangle_width, 0)));

				if (cache.completion_background_color.a > 0.01) {
					VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(completion_rect.position, completion_rect.size + Size2(scroll_rectangle_width, 0)), cache.completion_background_color);
				}
				VisualServer::get_singleton()->canvas_item_add_rect(ci, Rect2(Point2(completion_rect.position.x, completion_rect.position.y + (completion_index - line_from) * get_row_height()), Size2(completion_rect.size.width, get_row_height())), cache.completion_selected_color);
				draw_rect(Rect2(completion_rect.position + Vector2(icon_area_size.x + icon_hsep, 0), Size2(MIN(completion_base_width, completion_rect.size.width - (icon_area_size.x + icon_hsep)), completion_rect.size.height)), cache.completion_existing_color);

				for (int i = 0; i < row_count; i++) {
					int l = line_from + i;
					ERR_CONTINUE(l < 0 || l >= completion_options_size);
					Color text_color = cache.completion_font_color;
					for (int j = 0; j < color_regions.size(); j++) {
						if (completion_options[l].insert_text.begins_with(color_regions[j].begin_key)) {
							text_color = color_regions[j].color;
						}
					}
					int yofs = (get_row_height() - cache.font->get_height()) / 2;
					Point2 title_pos(completion_rect.position.x, completion_rect.position.y + i * get_row_height() + cache.font->get_ascent() + yofs);

					// Draw completion icon if it is valid.
					Ref<Texture> icon = completion_options[l].icon;
					Rect2 icon_area(completion_rect.position.x, completion_rect.position.y + i * get_row_height(), icon_area_size.width, icon_area_size.height);
					if (icon.is_valid()) {
						const real_t max_scale = 0.7f;
						const real_t side = max_scale * icon_area.size.width;
						real_t scale = MIN(side / icon->get_width(), side / icon->get_height());
						Size2 icon_size = icon->get_size() * scale;
						draw_texture_rect(icon, Rect2(icon_area.position + (icon_area.size - icon_size) / 2, icon_size));
					}

					title_pos.x = icon_area.position.x + icon_area.size.width + icon_hsep;

					if (completion_options[l].default_value.get_type() == Variant::COLOR) {
						draw_rect(Rect2(Point2(completion_rect.position.x + completion_rect.size.width - icon_area_size.x, icon_area.position.y), icon_area_size), (Color)completion_options[l].default_value);
					}

					draw_string(cache.font, title_pos, completion_options[l].display, text_color, completion_rect.size.width - (icon_area_size.x + icon_hsep));
				}

				if (scroll_rectangle_width) {
					// Draw a small scroll rectangle to show a position in the options.
					float r = (float)maxlines / completion_options_size;
					float o = (float)line_from / completion_options_size;
					draw_rect(Rect2(completion_rect.position.x + completion_rect.size.width, completion_rect.position.y + o * completion_rect.size.y, scroll_rectangle_width, completion_rect.size.y * r), scrollc);
				}

				completion_line_ofs = line_from;
			}

			// Check to see if the hint should be drawn.
			bool show_hint = false;
			if (is_cursor_visible && completion_hint != "") {
				if (completion_active) {
					if (completion_below && !callhint_below) {
						show_hint = true;
					} else if (!completion_below && callhint_below) {
						show_hint = true;
					}
				} else {
					show_hint = true;
				}
			}

			if (show_hint) {
				Ref<StyleBox> sb = get_stylebox("panel", "TooltipPanel");
				Ref<Font> font = cache.font;
				Color font_color = get_color("font_color", "TooltipLabel");

				int max_w = 0;
				int sc = completion_hint.get_slice_count("\n");
				int offset = 0;
				int spacing = 0;
				for (int i = 0; i < sc; i++) {
					String l = completion_hint.get_slice("\n", i);
					int len = font->get_string_size(l).x;
					max_w = MAX(len, max_w);
					if (i == 0) {
						offset = font->get_string_size(l.substr(0, l.find(String::chr(0xFFFF)))).x;
					} else {
						spacing += cache.line_spacing;
					}
				}

				Size2 size2 = Size2(max_w, sc * font->get_height() + spacing);
				Size2 minsize = size2 + sb->get_minimum_size();

				if (completion_hint_offset == -0xFFFF) {
					completion_hint_offset = cursor_pos.x - offset;
				}

				Point2 hint_ofs = Vector2(completion_hint_offset, cursor_pos.y - cursor_insert_offset_y - (get_row_height() - cache.font->get_height()) / 2) + callhint_offset;

				if (callhint_below) {
					hint_ofs.y += get_row_height() + sb->get_offset().y;
				} else {
					hint_ofs.y -= minsize.y + sb->get_offset().y;
				}

				draw_style_box(sb, Rect2(hint_ofs, minsize));

				spacing = 0;
				for (int i = 0; i < sc; i++) {
					int begin = 0;
					int end = 0;
					String l = completion_hint.get_slice("\n", i);

					if (l.find(String::chr(0xFFFF)) != -1) {
						begin = font->get_string_size(l.substr(0, l.find(String::chr(0xFFFF)))).x;
						end = font->get_string_size(l.substr(0, l.rfind(String::chr(0xFFFF)))).x;
					}

					Point2 round_ofs = hint_ofs + sb->get_offset() + Vector2(0, font->get_ascent() + font->get_height() * i + spacing);
					round_ofs = round_ofs.round();
					draw_string(font, round_ofs, l.replace(String::chr(0xFFFF), ""), font_color);
					if (end > 0) {
						Vector2 b = hint_ofs + sb->get_offset() + Vector2(begin, font->get_height() + font->get_height() * i + spacing - 1);
						draw_line(b, b + Vector2(end - begin, 0), font_color);
					}
					spacing += cache.line_spacing;
				}
			}

			if (has_focus()) {
				OS::get_singleton()->set_ime_active(true);
				OS::get_singleton()->set_ime_position(get_global_position() + cursor_pos + Point2(0, get_row_height()));
			}
		} break;
		case NOTIFICATION_FOCUS_ENTER: {
			if (caret_blink_enabled) {
				caret_blink_timer->start();
			} else {
				draw_caret = true;
			}

			OS::get_singleton()->set_ime_active(true);
			Point2 cursor_pos = Point2(cursor_get_column(), cursor_get_line()) * get_row_height();
			OS::get_singleton()->set_ime_position(get_global_position() + cursor_pos);

			if (OS::get_singleton()->has_virtual_keyboard() && virtual_keyboard_enabled) {
				int cursor_start = -1;
				int cursor_end = -1;

				if (!selection.active) {
					String full_text = _base_get_text(0, 0, cursor.line, cursor.column);

					cursor_start = full_text.length();
				} else {
					String pre_text = _base_get_text(0, 0, selection.from_line, selection.from_column);
					String post_text = _base_get_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);

					cursor_start = pre_text.length();
					cursor_end = cursor_start + post_text.length();
				}

				OS::get_singleton()->show_virtual_keyboard(get_text(), get_global_rect(), OS::KEYBOARD_TYPE_MULTILINE, -1, cursor_start, cursor_end);
			}
		} break;
		case NOTIFICATION_FOCUS_EXIT: {
			if (caret_blink_enabled) {
				caret_blink_timer->stop();
			}

			OS::get_singleton()->set_ime_position(Point2());
			OS::get_singleton()->set_ime_active(false);
			ime_text = "";
			ime_selection = Point2();

			if (OS::get_singleton()->has_virtual_keyboard() && virtual_keyboard_enabled) {
				OS::get_singleton()->hide_virtual_keyboard();
			}

			if (deselect_on_focus_loss_enabled && !popup_show) {
				deselect();
			}
			popup_show = false;
		} break;
		case MainLoop::NOTIFICATION_OS_IME_UPDATE: {
			if (has_focus()) {
				ime_text = OS::get_singleton()->get_ime_text();
				ime_selection = OS::get_singleton()->get_ime_selection();
				update();
			}
		} break;
		case Control::NOTIFICATION_DRAG_BEGIN: {
			selection.selecting_mode = Selection::MODE_NONE;
			drag_action = true;
			dragging_minimap = false;
			dragging_selection = false;
			can_drag_minimap = false;
			click_select_held->stop();
		} break;
		case Control::NOTIFICATION_DRAG_END: {
			if (is_drag_successful()) {
				if (selection.drag_attempt) {
					selection.drag_attempt = false;
					if (!readonly && !Input::get_singleton()->is_key_pressed(KEY_CONTROL)) {
						_remove_text(selection.from_line, selection.from_column, selection.to_line, selection.to_column);
						cursor_set_line(selection.from_line, false);
						cursor_set_column(selection.from_column);
						selection.active = false;
						selection.selecting_mode = Selection::MODE_NONE;
						update();
					} else if (deselect_on_focus_loss_enabled) {
						deselect();
					}
				}
			} else {
				selection.drag_attempt = false;
			}
			drag_action = false;
			drag_caret_force_displayed = false;
			dragging_minimap = false;
			dragging_selection = false;
			can_drag_minimap = false;
			click_select_held->stop();
		} break;
	}
}