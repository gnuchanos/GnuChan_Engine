/**************************************************************************/
/*  autocomplete.cpp                                                      */
/**************************************************************************/
/*  GCL Autocomplete modulu.                                              */
/*  - "@extern "  -> attach edilen node'un tipi (sinif adi)               */
/*  - "self."     -> @extern ile eklenmis node adlari                     */
/*  - "Input."    -> Input.IsPressed / IsUp / IsDown                      */
/*  - "Engine."   -> Engine.GetFPS / SetFPS / GetScreenSize / GetWindowSize */
/*  - diger       -> degiskenler + hazir fonksiyonlar                     */
/**************************************************************************/

#include "../gcl_script.h"
#include "../Parser/parser.h"

namespace gcl {

/* gcl_language.md -> Otomatik Tamamlama'sinda tanimli anahtar kelimeler. */
static const char *known_keywords[] = {
	"if", "elif", "else", "switch", "case", "default",
	"break", "continue", "return", "for", "while", "do",
	"int", "short", "float", "double", "long", "unsigned", "char", "bool",
	"void", "const", "sizeof", "struct", "enum", "typedef", "union", "static",
	"int8", "int16", "int32", "int64", "int128",
	"uint8", "uint16", "uint32", "uint64", "uint128",
	"float16", "float32", "float64", "float128",
	"gcChar", "gcMalloc", "public", "private", "global", "inline", "tuple", "dict",
	"scanf", "malloc", "free",
	"#include", "#lib", "#extern", "#register",
	"#define", "#undef", "#warning", "#error", "#debug",
	"#ifdef", "#ifndef", "#if", "#elif", "#else", "#endif",
};

/* gcl.md -> Hazir fonksiyonlar.
   SADECE PrintF. print/printf/Print kullanici istemedi - kaldirildi.
   Tamamlama sadece ismi yazar; "(" ve icerik otomatik eklENMEZ. */
static const char *known_functions[] = {
	"Ready()",
	"Update(float delta)",
	"UpdatePhysics(float delta)",
	"PrintF",
};

/* gcl.md -> 7) Input. */
static const char *input_members[] = {
	"IsPressed(\"e\")",
	"IsUp(\"e\")",
	"IsDown(\"e\")",
};

/* gcl.md -> 8) Engine. */
static const char *engine_members[] = {
	"GetFPS",
	"SetFPS(fps)",
	"GetScreenSize",
	"GetWindowSize",
};

/* Script'in attach edildigi node'un tipini (sinif adini) dondurur. */
static String get_owner_class_name(Object *p_owner) {
	if (p_owner && p_owner->has_method("get_class")) {
		Variant cls = p_owner->call("get_class");
		String s = cls;
		if (!s.empty()) {
			return s;
		}
	}
	return String();
}

Error autocomplete_run(const String &p_code, Object *p_owner, List<ScriptCodeCompletionOption> *r_options) {
	/* Imlecin oldugu satiri ayir. */
	int caret = p_code.rfind("\n");
	String prefix = (caret == -1) ? p_code : p_code.substr(caret + 1, p_code.length() - caret - 1);

	/* "@extern <bos>" -> script'in attach edildigi node'un tipini (sinif adini) oner. */
	if (prefix.begins_with("@extern") && (prefix == "@extern" || prefix.ends_with(" "))) {
		String class_name = get_owner_class_name(p_owner);
		if (!class_name.empty()) {
			r_options->push_back(ScriptCodeCompletionOption(class_name, ScriptCodeCompletionOption::KIND_NODE_PATH));
		}
		return OK;
	}

	/* self. sonrasi @extern node adlarini oner. */
	if (prefix.ends_with("self.")) {
		Vector<String> nodes;
		extern_scan(p_code, nodes);
		for (int i = 0; i < nodes.size(); i++) {
			r_options->push_back(ScriptCodeCompletionOption(nodes[i], ScriptCodeCompletionOption::KIND_MEMBER));
		}
		return OK;
	}

	/* Input. sonrasi Input uyeleri. */
	if (prefix.ends_with("Input.")) {
		for (int i = 0; i < (int)(sizeof(input_members) / sizeof(input_members[0])); i++) {
			r_options->push_back(ScriptCodeCompletionOption(input_members[i], ScriptCodeCompletionOption::KIND_FUNCTION));
		}
		return OK;
	}

	/* Engine. sonrasi Engine uyeleri. */
	if (prefix.ends_with("Engine.")) {
		for (int i = 0; i < (int)(sizeof(engine_members) / sizeof(engine_members[0])); i++) {
			r_options->push_back(ScriptCodeCompletionOption(engine_members[i], ScriptCodeCompletionOption::KIND_FUNCTION));
		}
		return OK;
	}

	/* Normal yazim: degiskenler + hazir fonksiyonlar.
	   Imlec onundeki kelimeye gore filtrele: "i" yazinca UpdatePhysics onerilmez. */
	String match = prefix;
	int last_space = match.rfind(" ");
	int last_tab = match.rfind("\t");
	if (last_tab > last_space) {
		last_space = last_tab;
	}
	if (last_space != -1) {
		match = match.substr(last_space + 1, match.length() - last_space - 1);
	}

	Vector<String> variables;
	variable_scan(p_code, variables);

	for (int i = 0; i < variables.size(); i++) {
		if (match.empty() || variables[i].begins_with(match)) {
			r_options->push_back(ScriptCodeCompletionOption(variables[i], ScriptCodeCompletionOption::KIND_VARIABLE));
		}
	}

	for (int i = 0; i < (int)(sizeof(known_keywords) / sizeof(known_keywords[0])); i++) {
		String kw = known_keywords[i];
		if (match.empty() || kw.begins_with(match)) {
			r_options->push_back(ScriptCodeCompletionOption(kw, ScriptCodeCompletionOption::KIND_PLAIN_TEXT));
		}
	}

	for (int i = 0; i < (int)(sizeof(known_functions) / sizeof(known_functions[0])); i++) {
		String fn = known_functions[i];
		if (match.empty() || fn.begins_with(match)) {
			r_options->push_back(ScriptCodeCompletionOption(fn, ScriptCodeCompletionOption::KIND_FUNCTION));
		}
	}

	return OK;
}

} // namespace gcl
