/**************************************************************************/
/*  gcl_preprocessor.cpp                                                   */
/**************************************************************************/
/*  GCL preprocessor (gcl_language.md "Dil Sozdizimi" bolumu).            */
/*                                                                        */
/*  Line-based pass that runs BEFORE the lexer/parser:                    */
/*    #include "x.gcsf" | <x.gcsf>   inline file                          */
/*    #lib      "x.gclib" | <x.gclib> inline library                      */
/*    #define NAME value / #undef NAME                                    */
/*    #ifdef / #ifndef / #if / #elif / #else / #endif                     */
/*    #warning "text", macro, ... -> yellow console message               */
/*    #error   "text", ...        -> red console message + parse failure  */
/*    #debug   "text", ...        -> blue console message                 */
/*    #register / #extern         -> external binding declarations        */
/*                                   (stripped)                           */
/*                                                                        */
/*  '#if' supports the platform tags from gcl_language.md                 */
/*  (gnuLinux, gnu_linux, gnu, linux, windows) plus any #define'd name,   */
/*  numeric literals, defined() and the ! && || operators.                */
/**************************************************************************/

#include "gcl_preprocessor.h"

#include "core/error_macros.h"
#include "core/list.h"
#include "core/map.h"
#include "core/os/file_access.h"
#include "core/os/memory.h"
#include "core/print_string.h"
#include "core/project_settings.h"
#include "core/vector.h"

/* ------------------------------------------------------------------ */
/*  Small text helpers                                                 */
/* ------------------------------------------------------------------ */

static bool pp_is_ident_start(CharType c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool pp_is_ident_char(CharType c) {
	return pp_is_ident_start(c) || (c >= '0' && c <= '9');
}

/* ------------------------------------------------------------------ */
/*  Directive argument extraction: "file" or <file>                    */
/* ------------------------------------------------------------------ */

static String pp_extract_path_arg(const String &p_arg, String *r_error, int p_line) {
	String a = p_arg.strip_edges();
	if (a.length() >= 2 && a[0] == '"' && a[a.length() - 1] == '"') {
		return a.substr(1, a.length() - 2);
	}
	if (a.length() >= 2 && a[0] == '<' && a[a.length() - 1] == '>') {
		return a.substr(1, a.length() - 2);
	}
	if (r_error && *r_error == "") {
		*r_error = "expected \"file\" or <file> (line " + itos(p_line) + ")";
	}
	return String();
}

/* ------------------------------------------------------------------ */
/*  File reading (res:// aware)                                        */
/* ------------------------------------------------------------------ */

static Error pp_read_file(const String &p_path, String &r_content, String *r_error, int p_line) {
	String cand = p_path;
	if (cand.begins_with("res://") && ProjectSettings::get_singleton()) {
		cand = ProjectSettings::get_singleton()->globalize_path(cand);
	}
	Error err;
	FileAccess *f = FileAccess::open(cand, FileAccess::READ, &err);
	if (!f && p_path.begins_with("res://")) {
		// Fallback: try the raw path even if globalize_path failed.
		f = FileAccess::open(p_path, FileAccess::READ, &err);
	}
	if (!f) {
		if (r_error && *r_error == "") {
			*r_error = "cannot open included file '" + p_path + "' (line " + itos(p_line) + ")";
		}
		return ERR_FILE_NOT_FOUND;
	}
	r_content = f->get_as_utf8_string();
	f->close();
	memdelete(f);
	return OK;
}

/* ------------------------------------------------------------------ */
/*  Message formatting for #warning / #error / #debug                  */
/*  gcl_language.md: #warning "text", "text", variable, variable       */
/*  -> each argument is emitted; identifiers are macro-expanded.       */
/* ------------------------------------------------------------------ */

static String pp_expand_macros(const String &p_line, const Map<String, String> &p_macros);

static String pp_build_message(const String &p_arg, const Map<String, String> &p_macros) {
	String a = p_arg.strip_edges();
	if (a.length() >= 2 && a[0] == '"' && a[a.length() - 1] == '"') {
		return a.substr(1, a.length() - 2);
	}
	return pp_expand_macros(a, p_macros);
}

/* ------------------------------------------------------------------ */
/*  Macro expansion (only outside strings/chars/comments)              */
/* ------------------------------------------------------------------ */

static String pp_expand_macros(const String &p_line, const Map<String, String> &p_macros) {
	if (p_macros.size() == 0) {
		return p_line;
	}
	String out;
	bool in_str = false;
	bool in_char = false;
	int i = 0;
	while (i < p_line.length()) {
		CharType c = p_line[i];
		if (c == '"' && !in_char) {
			in_str = !in_str;
			out += String::chr(c);
			i++;
			continue;
		}
		if (c == '\'' && !in_str) {
			in_char = !in_char;
			out += String::chr(c);
			i++;
			continue;
		}
		if (!in_str && !in_char && pp_is_ident_start(c)) {
			int start = i;
			while (i < p_line.length() && pp_is_ident_char(p_line[i])) {
				i++;
			}
			String name = p_line.substr(start, i - start);
			if (p_macros.has(name)) {
				out += p_macros[name];
			} else {
				out += name;
			}
			continue;
		}
		out += String::chr(c);
		i++;
	}
	return out;
}

/* ------------------------------------------------------------------ */
/*  Simple #if expression evaluator                                    */
/*  Grammar: or -> and -> not -> atom                                  */
/*  atom   : number | NAME (1 if defined, else 0) | defined(NAME)      */
/*           | defined NAME | ( expr )                                */
/* ------------------------------------------------------------------ */

struct PPCondParser {
	const String &s;
	int i;
	const Map<String, String> &macros;

	PPCondParser(const String &p_s, const Map<String, String> &p_macros)
			: s(p_s), i(0), macros(p_macros) {}

	void skip_ws() {
		while (i < s.length() && (s[i] == ' ' || s[i] == '\t')) {
			i++;
		}
	}

	bool has_defined(const String &p_name) const {
		return macros.has(p_name);
	}

	int parse_or() {
		int v = parse_and();
		skip_ws();
		while (i + 1 < s.length() && s[i] == '|' && s[i + 1] == '|') {
			i += 2;
			int r = parse_and();
			v = (v || r) ? 1 : 0;
			skip_ws();
		}
		return v;
	}

	int parse_and() {
		int v = parse_not();
		skip_ws();
		while (i + 1 < s.length() && s[i] == '&' && s[i + 1] == '&') {
			i += 2;
			int r = parse_not();
			v = (v && r) ? 1 : 0;
			skip_ws();
		}
		return v;
	}

	int parse_not() {
		skip_ws();
		if (i < s.length() && s[i] == '!') {
			i++;
			return parse_not() ? 0 : 1;
		}
		return parse_atom();
	}

	int parse_atom() {
		skip_ws();
		if (i < s.length() && s[i] == '(') {
			i++;
			int v = parse_or();
			skip_ws();
			if (i < s.length() && s[i] == ')') {
				i++;
			}
			return v;
		}
		// defined(NAME) / defined NAME
		if (s.substr(i, 7) == "defined") {
			int j = i + 7;
			bool is_func = j < s.length() && s[j] == '(';
			if (is_func) {
				j++;
			}
			while (j < s.length() && (s[j] == ' ' || s[j] == '\t')) {
				j++;
			}
			int ks = j;
			while (j < s.length() && pp_is_ident_char(s[j])) {
				j++;
			}
			String name = s.substr(ks, j - ks);
			if (is_func && j < s.length() && s[j] == ')') {
				j++;
			}
			i = j;
			return has_defined(name) ? 1 : 0;
		}
		// numeric literal
		if (i < s.length() && s[i] >= '0' && s[i] <= '9') {
			int ks = i;
			while (i < s.length() && ((s[i] >= '0' && s[i] <= '9') || s[i] == '.')) {
				i++;
			}
			return s.substr(ks, i - ks).to_int();
		}
		// identifier: defined macro -> 1
		if (i < s.length() && pp_is_ident_start(s[i])) {
			int start = i;
			while (i < s.length() && pp_is_ident_char(s[i])) {
				i++;
			}
			String name = s.substr(start, i - start);
			if (name == "true") {
				return 1;
			}
			if (name == "false") {
				return 0;
			}
			return has_defined(name) ? 1 : 0;
		}
		// unknown -> treat as 0 and skip
		if (i < s.length()) {
			i++;
		}
		return 0;
	}

	int parse() {
		return parse_or();
	}
};

/* ------------------------------------------------------------------ */
/*  Conditional compilation stack                                      */
/* ------------------------------------------------------------------ */

struct PPCondFrame {
	bool parent_active;
	bool active;
	bool taken;

	PPCondFrame() {
		parent_active = true;
		active = true;
		taken = false;
	}
};

/* ------------------------------------------------------------------ */
/*  Include bookkeeping                                                */
/* ------------------------------------------------------------------ */

static bool pp_already_included(const List<String> &p_included, const String &p_path) {
	for (const List<String>::Element *E = p_included.front(); E; E = E->next()) {
		if (E->get() == p_path) {
			return true;
		}
	}
	return false;
}

/* ------------------------------------------------------------------ */
/*  Core pass                                                          */
/* ------------------------------------------------------------------ */

static Error pp_process(const String &p_source, String *r_out, const String &p_self_path, int p_depth, Map<String, String> &r_macros, List<String> &r_included, String *r_error, int *r_err_line) {
	if (p_depth > 32) {
		if (r_error && *r_error == "") {
			*r_error = "include depth exceeded 32 (possible cyclic include)";
		}
		if (r_err_line) {
			*r_err_line = 1;
		}
		return ERR_PARSE_ERROR;
	}

	Vector<String> lines = p_source.split("\n");
	List<PPCondFrame> cond_stack;
	String out;
	bool in_block_comment = false;

	for (int li = 0; li < lines.size(); li++) {
		int line_no = li + 1;
		String raw = lines[li];
		String line = raw.strip_edges();

		// Block comments (#| ... |#) can span lines. While inside one, do
		// NOT macro-expand and do NOT interpret directives.
		if (in_block_comment) {
			out += raw + "\n";
			int end = raw.find("|#");
			if (end != -1) {
				in_block_comment = false;
			}
			continue;
		}
		if (line.begins_with("#|")) {
			out += raw + "\n";
			if (raw.find("|#") == -1) {
				in_block_comment = true;
			}
			continue;
		}

		bool parent_active = true;
		if (cond_stack.size() > 0) {
			parent_active = cond_stack.back()->get().active;
		}

		/* #include / #lib: inline the referenced file. */
		if (line.begins_with("#include") || line.begins_with("#lib")) {
			if (!parent_active) {
				continue;
			}
			String keyword = line.begins_with("#lib") ? "#lib" : "#include";
			String path = pp_extract_path_arg(line.substr(keyword.length()).strip_edges(), r_error, line_no);
			if (path == "") {
				if (r_error && *r_error == "") {
					*r_error = "malformed " + keyword + " (line " + itos(line_no) + ")";
				}
				if (r_err_line) {
					*r_err_line = line_no;
				}
				return ERR_PARSE_ERROR;
			}

			// Candidate paths: relative to the including file, then raw.
			Vector<String> candidates;
			if (p_self_path != "" && !path.begins_with("res://")) {
				int slash = p_self_path.rfind("/");
				if (slash >= 0) {
					candidates.push_back(p_self_path.substr(0, slash + 1) + path);
				}
			}
			candidates.push_back(path);

			bool found = false;
			for (int ci = 0; ci < candidates.size(); ci++) {
				String cand = candidates[ci];
				String content;
				Error re = pp_read_file(cand, content, r_error, line_no);
				if (re != OK) {
					continue;
				}
				if (pp_already_included(r_included, cand)) {
					found = true;
					break;
				}
				r_included.push_back(cand);
				Error pe = pp_process(content, &out, cand, p_depth + 1, r_macros, r_included, r_error, r_err_line);
				if (pe != OK) {
					return pe;
				}
				found = true;
				break;
			}
			if (!found) {
				if (r_error && *r_error == "") {
					*r_error = "cannot open included file '" + path + "' (line " + itos(line_no) + ")";
				}
				if (r_err_line) {
					*r_err_line = line_no;
				}
				return ERR_PARSE_ERROR;
			}
			continue;
		}

		/* #define / #undef */
		if (line.begins_with("#define ")) {
			if (!parent_active) {
				continue;
			}
			String rest = line.substr(8).strip_edges();
			int sp = 0;
			while (sp < rest.length() && pp_is_ident_char(rest[sp])) {
				sp++;
			}
			String name = rest.substr(0, sp);
			if (name == "") {
				if (r_error && *r_error == "") {
					*r_error = "expected macro name after #define (line " + itos(line_no) + ")";
				}
				if (r_err_line) {
					*r_err_line = line_no;
				}
				return ERR_PARSE_ERROR;
			}
			r_macros[name] = rest.substr(sp).strip_edges();
			continue;
		}
		if (line.begins_with("#undef ")) {
			if (!parent_active) {
				continue;
			}
			String name = line.substr(7).strip_edges();
			if (r_macros.has(name)) {
				r_macros.erase(name);
			}
			continue;
		}

		/* #ifdef / #ifndef / #if */
		if (line == "#ifdef" || line.begins_with("#ifdef ")) {
			bool cond = r_macros.has(line.substr(6).strip_edges());
			PPCondFrame f;
			f.parent_active = parent_active;
			f.active = parent_active && cond;
			f.taken = parent_active && cond;
			cond_stack.push_back(f);
			continue;
		}
		if (line == "#ifndef" || line.begins_with("#ifndef ")) {
			bool cond = !r_macros.has(line.substr(7).strip_edges());
			PPCondFrame f;
			f.parent_active = parent_active;
			f.active = parent_active && cond;
			f.taken = parent_active && cond;
			cond_stack.push_back(f);
			continue;
		}
		if (line.begins_with("#if ")) {
			String expr = line.substr(3).strip_edges();
			PPCondParser p(expr, r_macros);
			int v = p.parse();
			PPCondFrame f;
			f.parent_active = parent_active;
			f.active = parent_active && (v != 0);
			f.taken = parent_active && (v != 0);
			cond_stack.push_back(f);
			continue;
		}
		if (line.begins_with("#if") && !line.begins_with("#ifdef") && !line.begins_with("#ifndef")) {
			if (r_error && *r_error == "") {
				*r_error = "malformed #if (line " + itos(line_no) + ")";
			}
			if (r_err_line) {
				*r_err_line = line_no;
			}
			return ERR_PARSE_ERROR;
		}

		/* #elif / #else / #endif */
		if (line.begins_with("#elif ")) {
			if (cond_stack.size() == 0) {
				if (r_error && *r_error == "") {
					*r_error = "#elif without #if (line " + itos(line_no) + ")";
				}
				if (r_err_line) {
					*r_err_line = line_no;
				}
				return ERR_PARSE_ERROR;
			}
			PPCondFrame &f = cond_stack.back()->get();
			if (f.parent_active && !f.taken) {
				String expr = line.substr(5).strip_edges();
				PPCondParser p(expr, r_macros);
				int v = p.parse();
				f.active = (v != 0);
				if (v != 0) {
					f.taken = true;
				}
			} else {
				f.active = false;
			}
			continue;
		}
		if (line == "#else") {
			if (cond_stack.size() == 0) {
				if (r_error && *r_error == "") {
					*r_error = "#else without #if (line " + itos(line_no) + ")";
				}
				if (r_err_line) {
					*r_err_line = line_no;
				}
				return ERR_PARSE_ERROR;
			}
			PPCondFrame &f = cond_stack.back()->get();
			f.active = f.parent_active && !f.taken;
			if (f.active) {
				f.taken = true;
			}
			continue;
		}
		if (line == "#endif") {
			if (cond_stack.size() == 0) {
				if (r_error && *r_error == "") {
					*r_error = "#endif without #if (line " + itos(line_no) + ")";
				}
				if (r_err_line) {
					*r_err_line = line_no;
				}
				return ERR_PARSE_ERROR;
			}
			cond_stack.pop_back();
			continue;
		}

		/* #warning / #error / #debug: comma-separated args. */
		if (line.begins_with("#warning")) {
			if (!parent_active) {
				continue;
			}
			String args = line.substr(8).strip_edges();
			Vector<String> parts = args.split(",");
			String msg;
			for (int k = 0; k < parts.size(); k++) {
				if (k > 0) {
					msg += ", ";
				}
				msg += pp_build_message(parts[k], r_macros);
			}
			WARN_PRINT("[GCL-Warning] " + msg); // sari
			continue;
		}
		if (line.begins_with("#error")) {
			if (!parent_active) {
				continue;
			}
			String args = line.substr(6).strip_edges();
			Vector<String> parts = args.split(",");
			String msg;
			for (int k = 0; k < parts.size(); k++) {
				if (k > 0) {
					msg += ", ";
				}
				msg += pp_build_message(parts[k], r_macros);
			}
			ERR_PRINT("[GCL-Error] " + msg); // kirmizi
			if (r_error && *r_error == "") {
				*r_error = "#error: " + msg + " (line " + itos(line_no) + ")";
			}
			if (r_err_line) {
				*r_err_line = line_no;
			}
			return ERR_PARSE_ERROR;
		}
		if (line.begins_with("#debug")) {
			if (!parent_active) {
				continue;
			}
			String args = line.substr(6).strip_edges();
			Vector<String> parts = args.split(",");
			String msg;
			for (int k = 0; k < parts.size(); k++) {
				if (k > 0) {
					msg += ", ";
				}
				msg += pp_build_message(parts[k], r_macros);
			}
			print_line("[GCL-Debug] " + msg);
			continue;
		}

		/* #register / #extern: external library binding declarations.
		 * Converted to @register/@extern_native so the parser can collect
		 * them into GCLScriptData::extern_bindings. */
		if (line.begins_with("#register")) {
			if (!parent_active) {
				continue;
			}
			out += "@register " + line.substr(9).strip_edges() + "\n";
			continue;
		}
		if (line == "#extern" || line.begins_with("#extern ")) {
			if (!parent_active) {
				continue;
			}
			out += "@extern_native " + line.substr(7).strip_edges() + "\n";
			continue;
		}

		/* Inactive #if branch: skip everything. */
		if (!parent_active) {
			continue;
		}

		/* Ordinary '# comment' lines are kept for the lexer. */
		if (line.begins_with("#")) {
			out += raw + "\n";
			continue;
		}
		out += pp_expand_macros(raw, r_macros) + "\n";
	}

	if (in_block_comment) {
		if (r_error && *r_error == "") {
			*r_error = "unterminated block comment";
		}
		if (r_err_line) {
			*r_err_line = 1;
		}
		return ERR_PARSE_ERROR;
	}
	if (cond_stack.size() > 0) {
		if (r_error && *r_error == "") {
			*r_error = "unterminated #if (missing #endif)";
		}
		if (r_err_line) {
			*r_err_line = 1;
		}
		return ERR_PARSE_ERROR;
	}

	*r_out = out;
	return OK;
}

/* ------------------------------------------------------------------ */
/*  Entry                                                              */
/* ------------------------------------------------------------------ */

Error gcl_preprocess(const String &p_source, String *r_out, String *r_error, int *r_err_line) {
	if (!r_out) {
		return ERR_INVALID_PARAMETER;
	}
	Map<String, String> macros;
	List<String> included;

	// Pre-defined platform tags (gcl_language.md "#if"):
	//   gnuLinux, gnu_linux, gnu, linux, windows
#ifdef WINDOWS_ENABLED
	macros["windows"] = "1";
#endif
#ifdef LINUX_ENABLED
	macros["linux"] = "1";
	macros["gnu"] = "1";
	macros["gnu_linux"] = "1";
	macros["gnuLinux"] = "1";
#endif
#if defined(OSX_ENABLED) || defined(UNIX_ENABLED)
	macros["gnu"] = "1";
#endif

	return pp_process(p_source, r_out, "", 0, macros, included, r_error, r_err_line);
}
