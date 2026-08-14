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

void TextEdit::set_completion(bool p_enabled, const Vector<String> &p_prefixes) {
	completion_prefixes.clear();
	completion_enabled = p_enabled;
	for (int i = 0; i < p_prefixes.size(); i++) {
		completion_prefixes.insert(p_prefixes[i]);
	}
}

void TextEdit::_confirm_completion() {
	begin_complex_operation();

	_remove_text(cursor.line, cursor.column - completion_base.length(), cursor.line, cursor.column);
	cursor_set_column(cursor.column - completion_base.length(), false);
	insert_text_at_cursor(completion_current.insert_text);

	// When inserted into the middle of an existing string/method, don't add an unnecessary quote/bracket.
	String line = text[cursor.line];
	CharType next_char = line[cursor.column];
	CharType last_completion_char = completion_current.insert_text[completion_current.insert_text.length() - 1];
	CharType last_completion_char_display = completion_current.display[completion_current.display.length() - 1];

	if ((last_completion_char == '"' || last_completion_char == '\'') && (last_completion_char == next_char || last_completion_char_display == next_char)) {
		_remove_text(cursor.line, cursor.column, cursor.line, cursor.column + 1);
	}

	if (last_completion_char == '(') {
		if (next_char == last_completion_char) {
			_remove_text(cursor.line, cursor.column - 1, cursor.line, cursor.column);
		} else if (auto_brace_completion_enabled) {
			insert_text_at_cursor(")");
			cursor.column--;
		}
	} else if (last_completion_char == ')' && next_char == '(') {
		_remove_text(cursor.line, cursor.column - 2, cursor.line, cursor.column);
		if (line[cursor.column + 1] != ')') {
			cursor.column--;
		}
	}

	end_complex_operation();

	_cancel_completion();

	if (last_completion_char == '(') {
		query_code_comple();
	}
}

void TextEdit::_cancel_code_hint() {
	completion_hint = "";
	update();
}

void TextEdit::_cancel_completion() {
	if (!completion_active) {
		return;
	}

	completion_active = false;
	completion_forced = false;
	update();
}

static bool _is_completable(CharType c) {
	return !_is_symbol(c) || c == '"' || c == '\'';
}

void TextEdit::_update_completion_candidates() {
	String l = text[cursor.line];
	int cofs = CLAMP(cursor.column, 0, l.length());

	String s;

	// Look for keywords first.

	bool inquote = false;
	int first_quote = -1;
	int restore_quotes = -1;

	int c = cofs - 1;
	while (c >= 0) {
		if (l[c] == '"' || l[c] == '\'') {
			inquote = !inquote;
			if (first_quote == -1) {
				first_quote = c;
			}
			restore_quotes = 0;
		} else if (restore_quotes == 0 && l[c] == '$') {
			restore_quotes = 1;
		} else if (restore_quotes == 0 && !_is_whitespace(l[c])) {
			restore_quotes = -1;
		}
		c--;
	}

	bool pre_keyword = false;
	bool cancel = false;

	if (!inquote && first_quote == cofs - 1) {
		// No completion here.
		cancel = true;
	} else if (inquote && first_quote != -1) {
		s = l.substr(first_quote, cofs - first_quote);
	} else if (cofs > 0 && l[cofs - 1] == ' ') {
		int kofs = cofs - 1;
		String kw;
		while (kofs >= 0 && l[kofs] == ' ') {
			kofs--;
		}

		while (kofs >= 0 && l[kofs] > 32 && _is_completable(l[kofs])) {
			kw = String::chr(l[kofs]) + kw;
			kofs--;
		}

		pre_keyword = keywords.has(kw);

	} else {
		while (cofs > 0 && l[cofs - 1] > 32 && (l[cofs - 1] == '/' || _is_completable(l[cofs - 1]))) {
			s = String::chr(l[cofs - 1]) + s;
			if (l[cofs - 1] == '\'' || l[cofs - 1] == '"' || l[cofs - 1] == '$') {
				break;
			}

			cofs--;
		}
	}

	if (cursor.column > 0 && l[cursor.column - 1] == '(' && !pre_keyword && !completion_forced) {
		cancel = true;
	}

	update();

	bool prev_is_prefix = false;
	if (cofs > 0 && completion_prefixes.has(String::chr(l[cofs - 1]))) {
		prev_is_prefix = true;
	}
	// Check with one space before prefix, to allow indent.
	if (cofs > 1 && l[cofs - 1] == ' ' && completion_prefixes.has(String::chr(l[cofs - 2]))) {
		prev_is_prefix = true;
	}
	/* GCL zincir tamamlamasi: "self.Raycast." yazilip imlec noktadan sonra
	 * iken prefix bos olur (nokta semboldur). Iptal sartindan kacmak icin
	 * noktayi "prefixli" say: GCL'ye bos prefix + 'self.Raycast.' satiri
	 * gider, GCL zinciri tanir ve node uyelerini dondurur. GDScript'te
	 * nokta sonrasi tamamlama bu satirda acilmaz (GDScript kendi yolunu
	 * bildirir), dolayisiyla davranis bozulmaz. */
	if (cofs > 0 && l[cofs - 1] == '.') {
		prev_is_prefix = true;
	}

	if (cancel || (!pre_keyword && s == "" && (cofs == 0 || !prev_is_prefix))) {
		// None to complete, cancel.
		_cancel_completion();
		return;
	}

	completion_options.clear();
	completion_index = 0;
	completion_force_item_center = -1;
	completion_base = s;
	Vector<float> sim_cache;
	bool single_quote = s.begins_with("'");
	Vector<ScriptCodeCompletionOption> completion_options_casei;

	for (List<ScriptCodeCompletionOption>::Element *E = completion_sources.front(); E; E = E->next()) {
		ScriptCodeCompletionOption &option = E->get();

		if (single_quote && option.display.is_quoted()) {
			option.display = option.display.unquote().quote("'");
		}

		if (inquote && restore_quotes == 1 && !option.display.is_quoted()) {
			String quote = single_quote ? "'" : "\"";
			option.display = option.display.quote(quote);
			option.insert_text = option.insert_text.quote(quote);
		}

		if (option.display.begins_with(s)) {
			completion_options.push_back(option);
		} else if (option.display.to_lower().begins_with(s.to_lower())) {
			completion_options_casei.push_back(option);
		}
	}

	completion_options.append_array(completion_options_casei);

	if (completion_options.size() == 0) {
		for (int i = 0; i < completion_sources.size(); i++) {
			if (s.is_subsequence_of(completion_sources[i].display)) {
				completion_options.push_back(completion_sources[i]);
			}
		}
	}

	if (completion_options.size() == 0) {
		for (int i = 0; i < completion_sources.size(); i++) {
			if (s.is_subsequence_ofi(completion_sources[i].display)) {
				completion_options.push_back(completion_sources[i]);
			}
		}
	}

	if (completion_options.size() == 0) {
		// No options to complete, cancel.
		_cancel_completion();
		return;
	}

	if (completion_options.size() == 1 && s == completion_options[0].display) {
		// A perfect match, stop completion.
		_cancel_completion();
		return;
	}

	// The top of the list is the best match.
	completion_current = completion_options[0];
	completion_enabled = true;
}

void TextEdit::query_code_comple() {
	String l = text[cursor.line];
	int ofs = CLAMP(cursor.column, 0, l.length());

	bool inquote = false;

	int c = ofs - 1;
	while (c >= 0) {
		if (l[c] == '"' || l[c] == '\'') {
			inquote = !inquote;
		}
		c--;
	}

	bool ignored = completion_active && !completion_options.empty();
	if (ignored) {
		ScriptCodeCompletionOption::Kind kind = ScriptCodeCompletionOption::KIND_PLAIN_TEXT;
		const ScriptCodeCompletionOption *previous_option = nullptr;
		for (int i = 0; i < completion_options.size(); i++) {
			const ScriptCodeCompletionOption &current_option = completion_options[i];
			if (!previous_option) {
				previous_option = &current_option;
				kind = current_option.kind;
			}
			if (previous_option->kind != current_option.kind) {
				ignored = false;
				break;
			}
		}
		ignored = ignored && (kind == ScriptCodeCompletionOption::KIND_FILE_PATH || kind == ScriptCodeCompletionOption::KIND_NODE_PATH || kind == ScriptCodeCompletionOption::KIND_SIGNAL);
	}

	if (!ignored) {
		/* GCL: tamamlama istegi HER ZAMAN atilir (Ctrl+Space dahil).
		 * Eski kosul ('ofs > 0 && _is_completable(...)') su durumlarda
		 * sinyali yutuyordu:
		 *   - satir bos / satir basi (ofs == 0)
		 *   - imlec solunda sembol: 'if (|)' -> '(' ;  'self.|)' -> ')' vb.
		 * GCL bos girdiyle degisken + keyword + islev listesi dondurur;
		 * donen liste gercekten bossa CodeTextEditor popup'i acmaz. */
		emit_signal("request_completion");
	}
}

void TextEdit::set_code_hint(const String &p_hint) {
	completion_hint = p_hint;
	completion_hint_offset = -0xFFFF;
	update();
}

void TextEdit::code_complete(const List<ScriptCodeCompletionOption> &p_strings, bool p_forced) {
	completion_sources = p_strings;
	completion_active = true;
	completion_forced = p_forced;
	completion_current = ScriptCodeCompletionOption();
	completion_index = 0;
	completion_force_item_center = -1;
	_update_completion_candidates();
}
