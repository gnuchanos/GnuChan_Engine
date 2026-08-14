/**************************************************************************/
/*  autocomplete.cpp                                                      */
/**************************************************************************/
/*  GCL Autocomplete - gelismis tamamlama motoru.                         */
/*                                                                        */
/*  Senaryolar:                                                           */
/*    @extern <Sinif>        -> self.<Sinif> erisimi                      */
/*    self.                  -> kok uyeler + extern adlari                */
/*    self.<UyePref          -> kok uye on-eki filtreli                   */
/*    self.Raycast.          -> Raycast alt-uyeleri (alt extern)          */
/*    self.Camera. / Head.   -> Camera uyeleri                            */
/*    self.<extern>.         -> tablo uyeleri / genel node uyeleri        */
/*    <degisken>.            -> NODE/REF degisken uyeleri                 */
/*    Input. / Engine.       -> ozel tablolar                             */
/*    bos/genel              -> degisken + keyword + islev                */
/*                                                                        */
/*  Kritik nokta: TextEdit imleci get_text_for_completion()'da            */
/*  0xFFFF karakteriyle isaretler. Satir analizi DOSYANIN SON SATIRINA    */
/*  degil, IMLEÇ SATIRINA gore yapilir.                                   */
/**************************************************************************/

#include "../gcl_script.h"
#include "../Parser/parser.h"

namespace gcl {

/* ---------------------------------------------------------------- */
/*  Sabit listeler                                                  */
/* ---------------------------------------------------------------- */

static const char *const known_keywords[] = {
	"if", "elif", "else", "switch", "case", "default",
	"break", "continue", "return", "for", "while", "do",
	"int", "short", "float", "double", "long", "unsigned", "char", "bool",
	"void", "const", "sizeof", "struct", "enum", "typedef", "union", "static",
	"int8", "int16", "int32", "int64", "int128",
	"uint8", "uint16", "uint32", "uint64", "uint128",
	"float16", "float32", "float64", "float128",
	"gcChar", "gcMalloc", "public", "private", "global", "inline", "tuple", "dict",
	"Node", "NodeRef",
	"self",
	"scanf", "malloc", "free",
	"#include", "#lib", "#extern", "#register",
	"#define", "#undef", "#warning", "#error", "#debug",
	"#ifdef", "#ifndef", "#if", "#elif", "#else", "#endif",
};

static const char *const known_functions[] = {
	"Ready()",
	"Update(float delta)",
	"UpdatePhysics(float delta)",
	"PrintF",
};

static const char *const input_members[] = {
	"IsPressed(\"e\")",
	"IsUp(\"e\")",
	"IsDown(\"e\")",
};

static const char *const engine_members[] = {
	"GetFPS",
	"SetFPS(fps)",
	"GetScreenSize",
	"GetWindowSize",
};

/* FPSController kok uyeleri (self. sonrasi) - alt node adlari da icerir. */
static const char *const fps_root_members[] = {
	"MoveSpeed", "DuckSpeed", "RunSpeed", "JumpHeight",
	"StandingHeight", "CrouchHeight", "Gravity", "FallSpeed",
	"IsRunning", "IsDucking", "IsJumping", "IsPaused", "IsOnFloor",
	"Raycast", "Camera", "Head",
};

static const char *const raycast_members[] = {
	"IsColliding",
	"GetBodyName",
	"length",
	"Skip",
	"SkipList",
	"GetNode",
	"Free",
};

static const char *const camera_members[] = {
	"Fov",
	"MouseSpeed",
	"HeadBob",
	"HeadBobSpeed",
	"HeadBobAmount",
	"RunFovChange",
	"GetNode",
};

static const char *const time_members[] = {
	"Sleep(seconds)",
};

static const char *const node_members[] = {
	"Name",
	"Position",
	"Rotation",
	"Scale",
	"Transform",
	"Material",
	"Childs",
	"GetChild",
	"GetNode",
	"Find",
	"Hide",
	"Show",
	"Free",
	"Enable",
	"Disable",
};

/* ---------------------------------------------------------------- */
/*  Tablo arama                                                     */
/* ---------------------------------------------------------------- */

struct NodeMemberTable {
	const char *name;
	const char *const *members;
	int count;
};

#define MEMBER_TABLE(n, arr) \
	{ n, arr, (int)(sizeof(arr) / sizeof(arr[0])) }

static const NodeMemberTable node_tables[] = {
	MEMBER_TABLE("FPSController", fps_root_members),
	MEMBER_TABLE("Raycast", raycast_members),
	MEMBER_TABLE("RayCast", raycast_members),
	MEMBER_TABLE("Camera", camera_members),
	MEMBER_TABLE("Head", camera_members),
	MEMBER_TABLE("Node", node_members),
};

static const NodeMemberTable *find_member_table(const String &p_name) {
	String low = p_name.to_lower();
	for (int i = 0; i < (int)(sizeof(node_tables) / sizeof(node_tables[0])); i++) {
		if (low == String(node_tables[i].name).to_lower()) {
			return &node_tables[i];
		}
	}
	return nullptr;
}

/* ---------------------------------------------------------------- */
/*  Yardimcilar                                                     */
/* ---------------------------------------------------------------- */

/* TextEdit imlec isareti (0xFFFF) karsilastirmalari bozar; sil. */
static String strip_cursor_mark(const String &p_s) {
	String s = p_s;
	String mark = String::chr(CharType(0xFFFF));
	int idx = s.find(mark);
	while (idx != -1) {
		s = s.substr(0, idx) + s.substr(idx + 1, s.length() - idx - 1);
		idx = s.find(mark);
	}
	return s;
}

/* Bir listeyi filtreyle (p_partial bos = hepsi) r_options'a ekler. */
static void push_options(List<ScriptCodeCompletionOption> *r_options, const char *const *p_list, int p_count, const String &p_partial) {
	for (int i = 0; i < p_count; i++) {
		String item = p_list[i];
		if (p_partial.empty() || item.begins_with(p_partial)) {
			r_options->push_back(ScriptCodeCompletionOption(item, ScriptCodeCompletionOption::KIND_MEMBER));
		}
	}
}

/* Kullanici bir sub-node adini TAM OLMUS olarak mi yazdi?
   ('self.Raycast' gibi). Tam eslesmede kok uye listesi 'Raycast' zaten
   yazilmis oldugundan oneri VERILMEZ: TextEdit seed'i onaylayip
   'self.Raycast.Raycast' cift yazimi uretmesin.
   Onemli: KISA ONEKLERDE (R, Ra, Ray...) engel YOK - kullanici 'self.R'
   yazarken kok uyelerin filtreli onerisi gelmeli. */
static bool is_full_node_name(const String &p_partial_low) {
	static const char *sub_node_names[] = { "raycast", "camera", "head" };
	for (int i = 0; i < 3; i++) {
		if (p_partial_low == String(sub_node_names[i])) {
			return true;
		}
	}
	return false;
}

/* Imlec SOLUNDAKI son kelimeyi dondurur: yalnizca harf/rakam/_ karakterleri.
   'if (i' -> 'i', 'self.hello' -> 'hello', 'self.Raycast.' -> bos. */
static String get_last_word(const String &p_text) {
	int i = p_text.length();
	while (i > 0) {
		CharType c = p_text[i - 1];
		bool is_word = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') || c == '_';
		if (!is_word) {
			break;
		}
		i--;
	}
	return p_text.substr(i, p_text.length() - i);
}

/* ---------------------------------------------------------------- */
/*  self zincir tamamlama                                           */
/* ---------------------------------------------------------------- */

/* self. -> kok uyeler (extern tablosundan) + tablosuz extern adlari. */
static void complete_root_members(const Vector<String> &p_extern_names, List<ScriptCodeCompletionOption> *r_options, const String &p_partial) {
	const NodeMemberTable *root = nullptr;

	/* Ilk tablo eslesen @extern kok olarak kullanilir. */
	for (int i = 0; i < p_extern_names.size(); i++) {
		const NodeMemberTable *t = find_member_table(p_extern_names[i]);
		if (t != nullptr) {
			root = t;
			break;
		}
	}
	if (root == nullptr) {
		/* Varsayilan: FPSController (GnuChan FPS odakli motor). */
		root = find_member_table("FPSController");
	}

	push_options(r_options, root->members, root->count, p_partial);

	/* Tabloda karsiligi olmayan @extern adlari da kok listeye eklenir. */
	for (int i = 0; i < p_extern_names.size(); i++) {
		const String &n = p_extern_names[i];
		if (find_member_table(n) != nullptr) {
			continue;
		}
		if (p_partial.empty() || n.begins_with(p_partial)) {
			r_options->push_back(ScriptCodeCompletionOption(n, ScriptCodeCompletionOption::KIND_MEMBER));
		}
	}
}

static void complete_self_chain(const String &p_line, const Vector<String> &p_extern_names, List<ScriptCodeCompletionOption> *r_options) {
	int self_pos = p_line.findn("self.");
	if (self_pos == -1) {
		return;
	}
	String rest = p_line.substr(self_pos + 5); /* "self." sonrasi */
	String rest_low = rest.to_lower();

	/* --- self.<node>.<uye> --- */
	int dot = rest.find(".");
	if (dot != -1) {
		String node = rest.substr(0, dot);
		String member = rest.substr(dot + 1);

		const NodeMemberTable *table = find_member_table(node);

		/* Bilinmeyen node: script'te @extern olarak tanimliysa genel node uyeleri. */
		if (table == nullptr) {
			for (int i = 0; i < p_extern_names.size(); i++) {
				if (p_extern_names[i].to_lower() == node.to_lower()) {
					table = find_member_table("Node");
					break;
				}
			}
		}
		if (table == nullptr) {
			table = find_member_table("Node");
		}

		/* Uye on-eki hicbir uyeyle eslesmiyorsa (self.Raycast.MoveSpeed gibi)
		   TUM uyeleri goster; asla kok uyeler karismasin. */
		bool any = false;
		for (int mi = 0; mi < table->count; mi++) {
			if (String(table->members[mi]).begins_with(member)) {
				any = true;
				break;
			}
		}
		if (!any) {
			member = String();
		}
		push_options(r_options, table->members, table->count, member);
		return;
	}

	/* --- self. (bos) --- */
	if (rest.strip_edges().empty()) {
		complete_root_members(p_extern_names, r_options, String());
		return;
	}

	/* --- self.<on-ek> (nokta yok) --- */
	/* Sub-node adi TAM yazilmis ise oneri verme (cift yazim onlenir).
	   Kisa oneklerde (R, Ra...) engel yok: kok uyelerin filtreli onerisi gelir. */
	if (is_full_node_name(rest_low)) {
		return;
	}
	/* Kok uyelerin on-ek filtreli onerisi. */
	complete_root_members(p_extern_names, r_options, rest);
}

/* ---------------------------------------------------------------- */
/*  Girdi noktasi                                                   */
/* ---------------------------------------------------------------- */

Error autocomplete_run(const String &p_code, Object *p_owner, List<ScriptCodeCompletionOption> *r_options) {
	/* Imlec isareti (0xFFFF) TextEdit tarafindan TAM IMLEÇ KONUMUNA konur.
	   Analiz dosyanin son satirina degil, IMLEÇ SOLUNDAKI metne gore yapilir:
	     - 'if (self.|)'      -> left='if (self.'     -> self. kok uyeleri
	     - 'self.Raycast.|'   -> left='self.Raycast.' -> Raycast uyeleri
	     - 'if (i|nt)'        -> left='if (i'         -> match='i' -> int, if...
	   Imlec sagindaki karakterler (')', ';' vb.) filtreleri BOZMAZ. */
	String raw = p_code;
	int mark = raw.find(String::chr(CharType(0xFFFF)));
	bool has_mark = (mark != -1);
	if (!has_mark) {
		mark = raw.length();
	}

	int line_start = raw.rfind("\n", mark);
	int line_end = raw.find("\n", mark);
	String full_line = raw.substr(line_start + 1, (line_end == -1 ? raw.length() : line_end) - line_start - 1);

	int col = mark - (line_start + 1);
	if (col < 0) {
		col = 0;
	}
	if (col > full_line.length()) {
		col = full_line.length();
	}

	String left = (has_mark ? full_line.substr(0, col) : full_line);

	/* Taramalar (extern_scan, variable_scan) isaretsiz tam metin uzerinden. */
	String code = strip_cursor_mark(raw);

	Vector<String> extern_names;
	extern_scan(code, extern_names);

	/* @extern <bos> -> bagli node sinif adini oner. */
	if (left.begins_with("@extern") && (left == "@extern" || left.ends_with(" "))) {
		if (p_owner && p_owner->has_method("get_class")) {
			Variant cls = p_owner->call("get_class");
			String s = cls;
			if (!s.empty()) {
				r_options->push_back(ScriptCodeCompletionOption(s, ScriptCodeCompletionOption::KIND_NODE_PATH));
			}
		}
		return OK;
	}

	/* self. zinciri: @extern bildirimi olsun ya da olmasin calisir.
	   - self.Raycast. -> raycast uyeleri
	   - self.Camera./self.Head. -> camera uyeleri
	   - self.<extern>. -> tablo/node uyeleri
	   - self. -> kok uyeler + extern adlari
	   - self.<on-ek> -> filtreli oneri; node on-eki ise oneri yok. */
	{
		int self_pos = left.findn("self.");
		if (self_pos != -1) {
			complete_self_chain(left, extern_names, r_options);
			return OK;
		}
	}

	/* Input. sonrasi. */
	if (left.ends_with("Input.") || left.ends_with("Input.P")) {
		for (int i = 0; i < (int)(sizeof(input_members) / sizeof(input_members[0])); i++) {
			r_options->push_back(ScriptCodeCompletionOption(input_members[i], ScriptCodeCompletionOption::KIND_FUNCTION));
		}
		return OK;
	}

	/* Engine. sonrasi. */
	if (left.ends_with("Engine.") || left.ends_with("Engine.G")) {
		for (int i = 0; i < (int)(sizeof(engine_members) / sizeof(engine_members[0])); i++) {
			r_options->push_back(ScriptCodeCompletionOption(engine_members[i], ScriptCodeCompletionOption::KIND_FUNCTION));
		}
		return OK;
	}

	/* Time. sonrasi. */
	if (left.ends_with("Time.") || left.ends_with("Time.S")) {
		for (int i = 0; i < (int)(sizeof(time_members) / sizeof(time_members[0])); i++) {
			r_options->push_back(ScriptCodeCompletionOption(time_members[i], ScriptCodeCompletionOption::KIND_FUNCTION));
		}
		return OK;
	}

	/* Normal yazim: imlec solundaki SON KELIME (sembolsuz, + on-ek filtreli). */
	String match = get_last_word(left);

	/* "<degisken>.<uye-on-ek>" -> NODE/REF tipi degisken uyeleri.
	   'Body.'  -> tum uyeler (member bos)
	   'Body.Ro -> Rotation (on-ek filtreli)
	   'ref.x'  -> x ile baslayanlar
	   Obje adi bos olamaz; yalnizca imlec SOLUNDAKI zincir ele alinir. */
	{
		int last_dot = left.rfind(".");
		if (last_dot != -1) {
			String before = left.substr(0, last_dot).strip_edges();
			String obj = get_last_word(before);
			if (!obj.empty() && obj != "self") {
				String member = get_last_word(left);
				Vector<String> variables;
				variable_scan(code, variables);
				for (int i = 0; i < variables.size(); i++) {
					if (variables[i] == obj) {
						push_options(r_options, node_members, (int)(sizeof(node_members) / sizeof(node_members[0])), member);
						return OK;
					}
				}
			}
		}
	}

	Vector<String> variables;
	variable_scan(code, variables);

	/* GCL fonksiyon parametreleri (scoped): imlecin icinde bulundugu
	   fonksiyonun parametre adlari. Update(float delta) icindeyken yalnizca
	   o fonksiyonun delta'si onerilir; UpdatePhysics'in delta'si karismaz
	   (variable_scan artik imza satirlarini global listeye almaz). */
	Vector<String> params;
	function_params_scan(code, mark, params);
	for (int i = 0; i < params.size(); i++) {
		if (match.empty() || params[i].begins_with(match)) {
			r_options->push_back(ScriptCodeCompletionOption(params[i], ScriptCodeCompletionOption::KIND_VARIABLE));
		}
	}

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
