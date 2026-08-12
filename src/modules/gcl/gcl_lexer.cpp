/**************************************************************************/
/*  gcl_lexer.cpp                                                         */
/**************************************************************************/
/*  Tokenizer: raw source text -> GCLToken list.                          */
/*                                                                        */
/*  Handles: identifiers, numbers, strings (".."), chars ('..'),         */
/*  comments (# line, #| block |#), directives (#include ...), @extern,  */
/*  punctuators and operators.                                            */
/**************************************************************************/

#include "gcl_lexer.h"

static bool is_ident_start(CharType c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_ident_char(CharType c) {
	return is_ident_start(c) || (c >= '0' && c <= '9');
}

static bool is_digit(CharType c) {
	return c >= '0' && c <= '9';
}

/* Multi-char operator table (longest match first). */
static const char *gcl_operators[] = {
	"<<=", ">>=", "==", "!=", "<=", ">=", "&&", "||", "++", "--",
	"+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<", ">>",
	"->", "::",
	nullptr
};

static const char *gcl_punctuators = "(){}[],;.:?";
static const char *gcl_single_ops = "=+-*/%!~&|^<>";

Error gcl_lex(const String &p_source, List<GCLToken> *r_tokens, String *r_error, int *r_err_line) {
	if (!r_tokens) {
		return ERR_INVALID_PARAMETER;
	}
	r_tokens->clear();

	const int len = p_source.length();
	int i = 0;
	int line = 1;
	bool in_block_comment = false;
	int block_comment_line = 1;

	// Line-splitting pre-pass: the interpreter/parser work line-forward, and
	// GCL comments are start-of-line "#" or inline "#//". We tokenize per line
	// but keep a single token stream with line numbers.
	while (i < len) {
		CharType c = p_source[i];

		// newlines advance the line counter
		if (c == '\n') {
			line++;
			i++;
			continue;
		}
		// whitespace
		if (c == ' ' || c == '\t' || c == '\r') {
			i++;
			continue;
		}

		// block comment #| ... |#
		if (in_block_comment) {
			if (c == '|' && i + 1 < len && p_source[i + 1] == '#') {
				in_block_comment = false;
				block_comment_line = 0;
				i += 2;
			} else {
				if (c == '\n') {
					line++;
				}
				i++;
			}
			continue;
		}
		if (c == '#' && i + 1 < len && p_source[i + 1] == '|') {
			in_block_comment = true;
			block_comment_line = line;
			i += 2;
			continue;
		}

		// line comment "#" to end of line
		if (c == '#') {
			if (r_error && r_err_line) {
				// not an error: comments are legal
			}
			// skip to end of line
			while (i < len && p_source[i] != '\n') {
				i++;
			}
			continue;
		}

		// @extern ClassName -> special token
		if (c == '@') {
			int start = i;
			i++;
			while (i < len && is_ident_char(p_source[i])) {
				i++;
			}
			if (i == start + 1 && r_error) {
				*r_error = "expected identifier after '@'";
				if (r_err_line) {
					*r_err_line = line;
				}
				return ERR_PARSE_ERROR;
			}
			GCLToken t;
			t.type = GCL_TOKEN_EXTERN_CLASS;
			t.text = p_source.substr(start, i - start);
			t.line = line;
			r_tokens->push_back(t);
			continue;
		}

		// string literal
		if (c == '"') {
			String s;
			i++;
			bool closed = false;
			while (i < len) {
				CharType ch = p_source[i];
				if (ch == '\\' && i + 1 < len) {
					CharType n = p_source[i + 1];
					if (n == 'n') {
						s += '\n';
					} else if (n == 't') {
						s += '\t';
					} else if (n == 'r') {
						s += '\r';
					} else if (n == '\\') {
						s += '\\';
					} else if (n == '"') {
						s += '"';
					} else if (n == '\'') {
						s += '\'';
					} else {
						s += '\\';
						s += n;
					}
					i += 2;
					continue;
				}
				if (ch == '"') {
					i++;
					closed = true;
					break;
				}
				if (ch == '\n') {
					line++;
				}
				s += ch;
				i++;
			}
			if (!closed) {
				if (r_error) {
					*r_error = "unterminated string literal";
				}
				if (r_err_line) {
					*r_err_line = line;
				}
				return ERR_PARSE_ERROR;
			}
			GCLToken t;
			t.type = GCL_TOKEN_STRING;
			t.text = s;
			t.line = line;
			r_tokens->push_back(t);
			continue;
		}

		// char literal 'a'  (also: 'n' escape)
		if (c == '\'') {
			String s;
			i++;
			bool closed = false;
			if (i < len && p_source[i] == '\\' && i + 1 < len) {
				CharType n = p_source[i + 1];
				if (n == 'n') {
					s += '\n';
				} else if (n == 't') {
					s += '\t';
				} else if (n == '\\') {
					s += '\\';
				} else if (n == '\'') {
					s += '\'';
				} else {
					s += n;
				}
				i += 2;
			} else if (i < len) {
				s += p_source[i];
				i++;
			}
			if (i < len && p_source[i] == '\'') {
				i++;
				closed = true;
			}
			if (!closed) {
				if (r_error) {
					*r_error = "unterminated char literal";
				}
				if (r_err_line) {
					*r_err_line = line;
				}
				return ERR_PARSE_ERROR;
			}
			GCLToken t;
			t.type = GCL_TOKEN_CHAR;
			t.text = s;
			t.line = line;
			r_tokens->push_back(t);
			continue;
		}

		// directive '#...' handled above as comment; but gcl_language.md defines
		// #include/#lib/#extern/#define as directives. They start with '#' and
		// currently fall into the comment branch (whole-line). This is by
		// design: engine-level preprocessor/parser will process them line-wise.
		// Keep it simple for the minimal module set.

		// number literal
		if (is_digit(c) || (c == '.' && i + 1 < len && is_digit(p_source[i + 1]))) {
			int start = i;
			bool is_float = false;
			while (i < len) {
				CharType ch = p_source[i];
				if (is_digit(ch)) {
					i++;
				} else if (ch == '.') {
					is_float = true;
					i++;
				} else {
					break;
				}
			}
			// exponent: 1e5, 1.5e-3
			if (i < len && (p_source[i] == 'e' || p_source[i] == 'E')) {
				int j = i;
				if (j + 1 < len && (p_source[j + 1] == '+' || p_source[j + 1] == '-')) {
					j++;
				}
				if (j + 1 < len && is_digit(p_source[j + 1])) {
					is_float = true;
					j++;
					while (j < len && is_digit(p_source[j])) {
						j++;
					}
					i = j;
				}
			}
			GCLToken t;
			t.type = GCL_TOKEN_NUMBER;
			t.text = p_source.substr(start, i - start);
			t.line = line;
			r_tokens->push_back(t);
			continue;
		}

		// identifier / keyword
		if (is_ident_start(c)) {
			int start = i;
			while (i < len && is_ident_char(p_source[i])) {
				i++;
			}
			GCLToken t;
			t.type = GCL_TOKEN_IDENTIFIER;
			t.text = p_source.substr(start, i - start);
			t.line = line;
			r_tokens->push_back(t);
			continue;
		}

		// operators (longest match)
		bool matched_op = false;
		for (int o = 0; gcl_operators[o]; o++) {
			String op = gcl_operators[o];
			if (i + op.length() <= len && p_source.substr(i, op.length()) == op) {
				GCLToken t;
				t.type = GCL_TOKEN_OPERATOR;
				t.text = op;
				t.line = line;
				r_tokens->push_back(t);
				i += op.length();
				matched_op = true;
				break;
			}
		}
		if (matched_op) {
			continue;
		}

		// punctuators
			if (String(gcl_single_ops).find(String::chr(c)) != -1) {
		GCLToken t;
		t.type = GCL_TOKEN_OPERATOR;
		t.text = String::chr(c);
		t.line = line;
		r_tokens->push_back(t);
		i++;
		continue;
	}
if (String(gcl_punctuators).find(String::chr(c)) != -1) {
			GCLToken t;
			t.type = GCL_TOKEN_PUNCTUATOR;
			t.text = String::chr(c);
			t.line = line;
			r_tokens->push_back(t);
			i++;
			continue;
		}

		if (r_error) {
			*r_error = "unexpected character '" + String::chr(c) + "'";
		}
		if (r_err_line) {
			*r_err_line = line;
		}
		return ERR_PARSE_ERROR;
	}

	if (in_block_comment) {
		if (r_error) {
			*r_error = "unterminated block comment";
		}
		if (r_err_line) {
			*r_err_line = block_comment_line;
		}
		return ERR_PARSE_ERROR;
	}

	// EOF token
	GCLToken eof;
	eof.type = GCL_TOKEN_EOF;
	eof.line = line;
	r_tokens->push_back(eof);
	return OK;
}
