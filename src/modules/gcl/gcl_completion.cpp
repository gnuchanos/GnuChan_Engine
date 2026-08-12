/**************************************************************************/
/*  gcl_completion.cpp                                                    */
/**************************************************************************/
/*  Code completion for the GCL scripting language.                       */
/*                                                                        */
/*  Suggestions follow gcl.md exactly. A node's members are ONLY offered  */
/*  after you actually go to that node:                                   */
/*                                                                        */
/*  self.            -> @extern FPSController fields + its references     */
/*  self.Raycast.    -> Raycast members only (IsColliding/Skip/...)       */
/*  self.Camera.     -> Camera members only (Fov/MouseSpeed/...)          */
/*  self.Head.       -> Camera (child of Head)                            */
/*  self.Material.   -> Material properties                               */
/*  .GetNode/.Find/  -> generic node members (Name/Position/Material/...) */
/*  Engine.          -> GetFPS/SetFPS/GetScreenSize/GetWindowSize         */
/*  Input.           -> IsPressed/IsUp/IsDown                             */
/**************************************************************************/

#include "gcl_completion.h"

#include "core/print_string.h"
#include "scene/main/node.h"

static void gcl_add_opt(List<ScriptCodeCompletionOption> *r_options, const String &p_text, ScriptCodeCompletionOption::Kind p_kind) {
	r_options->push_back(ScriptCodeCompletionOption(p_text, p_kind));
}

static bool gcl_is_ident_char(CharType c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
}

/* The script's "@extern ClassName" decides what "self." offers.
 * @extern FPSController -> FPSController fields.
 * Any other @extern (Spatial, Mesh, ...) -> generic node members. */
static String gcl_script_extern_class(const String &p_src) {
	String src = p_src;
	int ex = src.find("@extern");
	if (ex == -1) {
		return String();
	}
	int start = ex + 7;
	while (start < src.length() && (src[start] == ' ' || src[start] == '\t' || src[start] == '\r' || src[start] == '\n')) {
		start++;
	}
	int end = start;
	while (end < src.length() && gcl_is_ident_char(src[end])) {
		end++;
	}
	return src.substr(start, end - start);
}

/* @extern FPSController (gcl.md): only the fields of this controller plus
 * its drag-drop references. No generic node members here. */
static void gcl_add_fpscontroller_members(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"MoveSpeed", "DuckSpeed", "RunSpeed", "JumpHeight",
		"StandingHeight", "CrouchHeight", "Gravity", "FallSpeed",
		"IsRunning", "IsDucking", "IsJumping", "IsPaused", "IsOnFloor",
		"Raycast", "Camera", "Head",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

/* @extern Raycast (gcl.md): only the raycast members. */
static void gcl_add_raycast_members(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"IsColliding", "GetBodyName", "length", "Skip", "SkipList", "GetNode", "Free",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

/* @extern Camera (gcl.md): only the camera members. */
static void gcl_add_camera_members(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"Fov", "MouseSpeed", "HeadBob", "HeadBobSpeed", "HeadBobAmount", "RunFovChange", "GetNode",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

/* @extern Mesh material block (gcl.md): self.Material. */
static void gcl_add_material_members(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"Color", "Albedo",
		"Metallic", "Metallic.Specular", "Metallic.Texture",
		"Roughness", "Roughness.Texture",
		"Emission", "EmissionColor", "EmissionTexture",
		"NormalMap.Enable", "NormalMap.Scale", "NormalMap.Texture",
		"AO", "AO.Texture",
		"Opacity", "Transparent", "CastShadows", "DoubleSided", "CullBackface",
		"BlendMode", "ShadingMode",
		"UV1Offset", "UV1Scale",
		"Detail.Albedo", "Detail.Roughness", "Detail.Normal",
		"Clear", "Copy", "Save", "Load",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

/* @extern Spatial / plain node referenced directly as self:
 * the node's own transform + the entry points to reach other nodes
 * (GetNode/Find). Name/Childs/GetChild/Material only make sense AFTER
 * going to a node via GetNode/Find/GetChild (gcl.md ERISIM KURALI). */
static void gcl_add_node_self(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"Position", "Rotation", "Scale", "Transform",
		"GetNode", "Find",
		"Hide", "Show", "Free", "Enable", "Disable",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

/* A node returned by GetNode/GetChild/Find: it can be any type, so offer
 * the generic node members described by gcl.md (Mesh block + node ops). */
static void gcl_add_generic_node(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"Name", "Position", "Rotation", "Scale", "Transform", "Material",
		"Childs", "GetChild", "GetNode", "Find",
		"Hide", "Show", "Free", "Enable", "Disable",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

/* gcl_language.md keywords: control flow, vanilla + GCL types, standard
 * functions and preprocessor macros. Offered even without a dot. */
static void gcl_add_gcl_keywords(List<ScriptCodeCompletionOption> *r_options) {
	static const char *segs[] = {
		"if", "elif", "else", "switch", "case", "default",
		"break", "continue", "return", "for", "while", "do",
		"int", "short", "float", "double", "long", "unsigned", "char", "bool",
		"void", "const", "sizeof", "struct", "enum", "typedef", "union", "static",
		"int8", "int16", "int32", "int64", "int128",
		"uint8", "uint16", "uint32", "uint64", "uint128",
		"float16", "float32", "float64", "float128",
		"gcChar", "gcMalloc", "public", "private", "global", "inline", "tuple", "dict",
		"printf", "scanf", "malloc", "free",
		"#include", "#lib", "#extern", "#register",
		"#define", "#undef", "#warning", "#error", "#debug",
		"#ifdef", "#ifndef", "#if", "#elif", "#else", "#endif",
		nullptr
	};
	for (int i = 0; segs[i]; i++) {
		gcl_add_opt(r_options, segs[i], ScriptCodeCompletionOption::KIND_MEMBER);
	}
}

Error gcl_complete_code(const String &p_code, const String &p_path, Object *p_owner, List<ScriptCodeCompletionOption> *r_options, bool &r_force, String &r_call_hint) {
	// The editor embeds a cursor marker (0xFFFF) where the caret is.
	// Only text before the caret has been typed, so cut everything after it.
	String code = p_code;
	int cursor = code.find(String::chr(0xFFFF));
	if (cursor != -1) {
		code = code.substr(0, cursor);
	}

	// Only the caret line matters; everything after it is not typed yet.
	int nl = code.find_last("\n");
	if (nl != -1) {
		code = code.substr(nl + 1);
	}
	code = code.strip_edges();

	int dot = code.find_last(".");
	if (dot == -1) {
		// No dot on the line yet: still trigger completion so the user can
		// start from the language entry points ("self." / Engine / ...).
		String typed_word = code;
		List<ScriptCodeCompletionOption> all;
		gcl_add_gcl_keywords(&all);
		gcl_add_opt(&all, "self", ScriptCodeCompletionOption::KIND_MEMBER);
		gcl_add_opt(&all, "Engine", ScriptCodeCompletionOption::KIND_MEMBER);
		gcl_add_opt(&all, "Input", ScriptCodeCompletionOption::KIND_MEMBER);
		gcl_add_opt(&all, "GetNode", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "FindNode", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "printf", ScriptCodeCompletionOption::KIND_FUNCTION);
		for (const List<ScriptCodeCompletionOption>::Element *E = all.front(); E; E = E->next()) {
			const ScriptCodeCompletionOption &opt = E->get();
			if (typed_word == "" || opt.insert_text.begins_with(typed_word)) {
				r_options->push_back(opt);
			}
		}
		if (r_options->size() > 0) {
			r_force = true;
		}
		return OK;
	}
	String base = code.substr(0, dot).strip_edges();
	String typed = code.substr(dot + 1).strip_edges();

	// The caret can sit inside an expression: "if (self.Raycast." or
	// "var x = self.Camera." -> cut everything up to the last delimiter
	// so "base" is just the expression we are completing.
	int cut = base.find_last("(");
	{
		int c = base.find_last("=");
		if (c > cut) cut = c;
		c = base.find_last(",");
		if (c > cut) cut = c;
		c = base.find_last("{");
		if (c > cut) cut = c;
		c = base.find_last("}");
		if (c > cut) cut = c;
		c = base.find_last(";");
		if (c > cut) cut = c;
		c = base.find_last(" ");
		if (c > cut) cut = c;
	}
	if (cut != -1) {
		base = base.substr(cut + 1).strip_edges();
	}

	List<ScriptCodeCompletionOption> all;

	if (base == "Engine") {
		gcl_add_opt(&all, "GetFPS", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "SetFPS", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "GetScreenSize", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "GetWindowSize", ScriptCodeCompletionOption::KIND_FUNCTION);
	} else if (base == "Input") {
		gcl_add_opt(&all, "IsPressed", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "IsUp", ScriptCodeCompletionOption::KIND_FUNCTION);
		gcl_add_opt(&all, "IsDown", ScriptCodeCompletionOption::KIND_FUNCTION);
	} else if (base == "self" || base.begins_with("self.")) {
		if (base == "self") {
			// self. -> the script's own @extern block decides what to offer.
			// @extern FPSController             -> FPSController fields
			// any other @extern (Spatial, ...)  -> generic node members
			String extern_class = gcl_script_extern_class(p_code);
			if (extern_class == "FPSController") {
				gcl_add_fpscontroller_members(&all);
			} else {
				gcl_add_node_self(&all);
			}
		} else {
			// self.<something>. -> go to that node first, then see its
			// OWN members. Nothing is offered without going to the node.
			int last_dot = base.find_last(".");
			String node_name = base.substr(last_dot + 1);
			// Only offer the members of a node AFTER that node was actually
			// reached through GetNode / GetChild / Find.
			if (node_name == "Material") {
				gcl_add_material_members(&all);
			} else if (node_name.begins_with("Find") || node_name.begins_with("GetNode") || node_name.begins_with("GetChild") || node_name.find("(") != -1) {
				gcl_add_generic_node(&all);
			} else if (node_name == "Raycast" || node_name.find("ray") != -1 || node_name.find("Ray") != -1) {
				gcl_add_raycast_members(&all);
			} else if (node_name == "Camera" || node_name.find("cam") != -1 || node_name.find("Cam") != -1) {
				gcl_add_camera_members(&all);
			} else if (node_name == "Head") {
				gcl_add_opt(&all, "Camera", ScriptCodeCompletionOption::KIND_NODE_PATH);
			} else {
				// Reached by a chain like self.Head.Camera: the last known
				// node decides. If unknown, offer nothing rather than junk.
			}
		}
	}

	// Filter by what has been typed after the dot.
	for (const List<ScriptCodeCompletionOption>::Element *E = all.front(); E; E = E->next()) {
		const ScriptCodeCompletionOption &opt = E->get();
		if (typed == "" || opt.insert_text.begins_with(typed)) {
			r_options->push_back(opt);
		}
	}

	if (r_options->size() > 0) {
		r_force = true;
	}
	return OK;
}
