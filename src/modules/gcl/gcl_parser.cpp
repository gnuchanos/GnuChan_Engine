/**************************************************************************/
/*  gcl_parser.cpp                                                        */
/**************************************************************************/
/*  Parser: source text + tokens -> GCLScriptData.                        */
/*                                                                        */
/*  Line-oriented function/statement splitting (keeps raw body lines for  */
/*  the interpreter) plus top-level member initializers and @extern name. */
/**************************************************************************/

#include "gcl_parser.h"

#include "core/node_path.h"
#include "gcl_lexer.h"
#include "gcl_preprocessor.h"

static Vector<String> parse_arg_list(const String &p_args_str) {
	Vector<String> args;
	Vector<String> parts = p_args_str.split(",");
	for (int i = 0; i < parts.size(); i++) {
		String p = parts[i].strip_edges();
		if (p == "") {
			continue;
		}
		Vector<String> words = p.split(" ");
		String last;
		for (int j = 0; j < words.size(); j++) {
			if (words[j] != "") {
				last = words[j];
			}
		}
		args.push_back(last);
	}
	return args;
}

static Variant parse_member_init(const String &p_rhs) {
	String v = p_rhs.strip_edges();
	if (v == "true") {
		return Variant(true);
	}
	if (v == "false") {
		return Variant(false);
	}
	if (v == "NULL" || v == "null" || v == "nullptr") {
		return Variant();
	}
	// $"path" -> NodePath (node references like self.Camera)
	if (v.begins_with("$\"") && v.length() >= 3 && v[v.length() - 1] == '"') {
		return Variant(NodePath(v.substr(2, v.length() - 3)));
	}
	// quoted string literal
	if (v.length() >= 2 && v[0] == '"' && v[v.length() - 1] == '"') {
		return Variant(v.substr(1, v.length() - 2));
	}
	if (v.length() >= 2 && v[0] == '\'' && v[v.length() - 1] == '\'') {
		return Variant(v.substr(1, v.length() - 2));
	}
	if (v.is_numeric() || v.is_valid_float()) {
		if (v.find(".") != -1 || v.find("e") != -1 || v.find("E") != -1) {
			return Variant(v.to_float());
		}
		return Variant(v.to_int());
	}
	return Variant();
}

/* Extracts the member name from a top-level declaration such as
 * "bool HandFull = false;" / "NODE OBJ = NULL;" — the identifier that
 * precedes '='. */
static String parse_member_name(const String &p_line) {
	int eq = p_line.find("=");
	if (eq == -1) {
		return "";
	}
	String lhs = p_line.substr(0, eq);
	String name;
	Vector<String> words = lhs.split(" ");
	for (int j = 0; j < words.size(); j++) {
		String w = words[j].strip_edges();
		if (w != "") {
			name = w;
		}
	}
	return name;
}

/* Counts '{' minus '}' in p_line starting at p_from, ignoring braces
 * inside string/char literals so that printf("{") cannot corrupt the
 * block count. */
static int count_braces(const String &p_line, int p_from = 0) {
	int count = 0;
	bool in_str = false;
	bool in_char = false;
	for (int i = p_from; i < p_line.length(); i++) {
		CharType c = p_line[i];
		if (c == '"') {
			in_str = !in_str;
		} else if (c == '\'') {
			in_char = !in_char;
		} else if (!in_str && !in_char) {
			if (c == '{') {
				count++;
			} else if (c == '}') {
				count--;
			}
		}
	}
	return count;
}

Error gcl_parse(const String &p_source, GCLScriptData *r_data, String *r_error, int *r_err_line) {
	if (!r_data) {
		return ERR_INVALID_PARAMETER;
	}
	r_data->functions.clear();
	r_data->members.clear();
	r_data->extern_class = "";
	r_data->extern_bindings.clear();
	r_data->top_level_lines.clear();

	// Run the preprocessor first: #include/#lib/#define/#if/#error/...
	// (gcl_language.md "Dil Sozdizimi" bolumu). It may fail with #error
	// or a missing include, before we ever tokenize.
	String processed;
	Error pe = gcl_preprocess(p_source, &processed, r_error, r_err_line);
	if (pe != OK) {
		return pe;
	}

	// Run the lexer second: it validates string/char/comment syntax on the
	// preprocessed source.
	List<GCLToken> tokens;
	Error le = gcl_lex(processed, &tokens, r_error, r_err_line);
	if (le != OK) {
		return le;
	}

	// Line-oriented pass. The interpreter executes function bodies line by
	// line, so we keep raw body lines (comment-stripped) rather than a
	// token-level AST.
	Vector<String> lines = processed.split("\n");
	int i = 0;
	while (i < lines.size()) {
		String line = lines[i].strip_edges();
		if (line == "" || line.begins_with("#")) {
			i++;
			continue;
		}
		if (line.begins_with("@regist") || line.begins_with("@extern_native")) {
			// External binding directives produced by the preprocessor:
			//   @register void InitWindow(int w, int h, const char *title);
			//   @extern_native "raylib.dll"
			// Collected into GCLScriptData::extern_bindings so the
			// runtime can resolve registered function names.
			GCLExternBinding b;
			if (line.begins_with("@regist")) {
				b.signature = line.substr(9).strip_edges();
				if (r_data->extern_bindings.size() > 0) {
					b.library_native = r_data->extern_bindings.back()->get().library_native;
				}
			} else {
				b.library_native = line.substr(14).strip_edges();
			}
			r_data->extern_bindings.push_back(b);
			i++;
			continue;
		}
		if (line.begins_with("@extern")) {
			String rest = line.substr(7).strip_edges();
			Vector<String> words = rest.split(" ");
			if (words.size() > 0) {
				r_data->extern_class = words[0];
			}
			i++;
			continue;
		}
		// Top-level member declaration ("bool HandFull = false;" /
		// "NODE OBJ = NULL;"): recorded so the instance can seed its
		// member variables before any function runs.
		if (line.find("(") == -1 && (line.find("=") != -1 || line.ends_with(";"))) {
			int eq = line.find("=");
			if (eq != -1) {
				GCLMemberData md;
				md.name = StringName(parse_member_name(line));
				if (md.name != StringName()) {
					String rhs = line.substr(eq + 1);
					int semi = rhs.find(";");
					if (semi != -1) {
						rhs = rhs.substr(0, semi);
					}
					md.init = parse_member_init(rhs);
					r_data->members.push_back(md);
				}
			}
			i++;
			continue;
		}
		int open = line.find("(");
		int close = line.find(")");
		if (open == -1 || close == -1 || close < open) {
			// Top-level statement that is not a member declaration. GCL
			// authors keep documentation/experiment one-liners at file scope
			// ("Input.IsDown('e')", "self.Raycast.Skip(self)") and a hard
			// parse error here made the WHOLE script invalid, so callbacks
			// silently stopped working. Only lines that clearly start with a
			// type keyword are treated as failed declarations; everything
			// else is skipped.
			Vector<String> first_words = line.split(" ");
			bool decl_like = first_words.size() > 0;
			if (decl_like) {
				String first = first_words[0].strip_edges();
				decl_like = first == "void" || first == "int" || first == "float" ||
						first == "bool" || first == "char" || first == "double" ||
						first == "String" || first == "NODE" || first == "Object";
			}
			if (decl_like) {
				if (r_error) {
					*r_error = "expected function declaration, got '" + line + "'";
				}
				if (r_err_line) {
					*r_err_line = i + 1;
				}
				return ERR_PARSE_ERROR;
			}
			// GCL 101: file-scope statements ("print_this_global();",
			// "Input.IsDown('e')"). Recorded so the instance runs them
			// once at instantiation time.
			r_data->top_level_lines.push_back(line);
			i++;
			continue;
		}

		// Function declaration: "<type> <name>(args)". The return type can be
		// void, bool, int, float, or any identifier; the name is the last
		// word before '('. A line with balanced parens but without a type +
		// name pair is a top-level call/loop ("for (i in X)", "Input.IsDown",
		// "self.Raycast.Skip") and is skipped instead of failing the file.
		String fname;
		Vector<String> decl_parts = line.substr(0, open).strip_edges().split(" ");
		if (decl_parts.size() >= 2) {
			fname = decl_parts[decl_parts.size() - 1].strip_edges();
		}
		if (decl_parts.size() < 2 || fname == "") {
			i++;
			continue;
		}

		GCLFunctionData fd;
		fd.name = fname;
		fd.line = i + 1;
		fd.arg_names = parse_arg_list(line.substr(open + 1, close - open - 1));

		// Count both '{' and '}' on the declaration line so single-line
		// bodies ("void Foo() { printf("hi"); }") close the block without
		// swallowing the following source lines.
		int brace_count = count_braces(line, close + 1);
		if (brace_count <= 0) {
			i++;
			r_data->functions.push_back(fd);
			continue;
		}
		i++;
		while (i < lines.size()) {
			String bl = lines[i];
			brace_count += count_braces(bl);
			if (brace_count <= 0) {
				i++;
				break;
			}
			fd.body_lines.push_back(bl);
			i++;
		}
		if (brace_count > 0) {
			if (r_error) {
				*r_error = "unbalanced braces in function '" + fname + "'";
			}
			if (r_err_line) {
				*r_err_line = fd.line;
			}
			return ERR_PARSE_ERROR;
		}

		r_data->functions.push_back(fd);
	}
	return OK;
}

void gcl_parse_functions(const String &p_source, List<GCLFunctionData> *r_functions, String *r_extern_class, List<GCLMemberData> *r_members) {
	GCLScriptData data;
	if (gcl_parse(p_source, &data) != OK) {
		return;
	}
	if (r_extern_class) {
		*r_extern_class = data.extern_class;
	}
	if (r_functions) {
		for (const List<GCLFunctionData>::Element *E = data.functions.front(); E; E = E->next()) {
			r_functions->push_back(E->get());
		}
	}
	if (r_members) {
		for (const List<GCLMemberData>::Element *E = data.members.front(); E; E = E->next()) {
			r_members->push_back(E->get());
		}
	}
}
