/**************************************************************************/
/*  gcl_loader.cpp                                                        */
/**************************************************************************/
/*  ResourceFormatLoader for GCL files:                                   */
/*    .gcsf   normal GCL script                                           */
/*    .gclib  modular GCL library                                         */
/*  Reads the file text and produces a GCLScript resource.                */
/**************************************************************************/

#include "gcl_loader.h"
#include "core/io/resource_saver.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"
#include "gcl_script.h"
#include "Lexer/lexer.h"

namespace gcl {

namespace {

/* "#include "path"" / "#include <path>" satirlarini dosya icerigiyle degistirir
   (metin birleştirme; recursive). strip_comments '#' satirlarini yorum sandigi
   icin include cozumu YORUM TEMIZLEMEDEN ONCE yapilmalidir. Derinlik siniri
   icin ic ice include'lar 16 duzeyle sinirlidir. */
String include_resolve(const String &p_code, const String &p_dir, int r_depth) {
	if (r_depth > 16) {
		return p_code;
	}

	StringBuilder out;
	int line_start = 0;
	const int len = p_code.length();

	while (line_start <= len) {
		int line_end = p_code.find("\n", line_start);
		if (line_end == -1) {
			line_end = len;
		}
		String line = p_code.substr(line_start, line_end - line_start);
		String trimmed = line.strip_edges();

		if (trimmed.begins_with("#include ")) {
			String rest = trimmed.substr(9).strip_edges();
			String include_path;
			if (rest.length() >= 2 && rest[0] == '"') {
				int close = rest.find("\"", 1);
				if (close != -1) {
					include_path = rest.substr(1, close - 1);
				}
			} else if (rest.length() >= 2 && rest[0] == '<') {
				int close = rest.find(">", 1);
				if (close != -1) {
					include_path = rest.substr(1, close - 1);
				}
			}

			if (!include_path.empty()) {
				String full = include_path;
				if (!include_path.begins_with("res://") && !include_path.begins_with("user://")) {
					full = p_dir + include_path;
				} else {
					full = ProjectSettings::get_singleton()->globalize_path(include_path);
				}
				FileAccess *inc = FileAccess::open(full, FileAccess::READ);
				if (inc) {
					String inc_code = inc->get_as_utf8_string();
					inc->close();
					memdelete(inc);
					out += include_resolve(inc_code, p_dir, r_depth + 1);
					out += "\n";
				} else {
					ERR_PRINT("GCL: include file not found: '" + full + "'.");
				}
			}
		} else {
			out += line;
			if (line_end < len) {
				out += "\n";
			}
		}

		if (line_end == len) {
			break;
		}
		line_start = line_end + 1;
	}
	return out.as_string();
}

} // namespace

RES ResourceFormatLoaderGCL::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_no_subresource_cache) {
	FileAccess *file = FileAccess::open(p_path, FileAccess::READ);
	if (!file) {
		if (r_error) {
			*r_error = ERR_CANT_OPEN;
		}
		return RES();
	}

	String code = file->get_as_utf8_string();
	memdelete(file);

	/* Include stage: "#include "lib.gclib"" satirlarini icerige gom (yorum
	   temizlemeden ONCE; strip_comments '#'i yorum sanar). */
	String dir = p_path.get_base_dir();
	if (!dir.empty() && !dir.ends_with("/")) {
		dir += "/";
	}
	code = include_resolve(code, dir, 0);

	/* Comment stage: yorumlari ayikla. */
	code = gcl::strip_comments(code);

	Ref<GCLScript> script;
	script.instance();
	script->set_source_code(code);
	script->set_path(p_path);

	if (r_error) {
		*r_error = OK;
	}
	return script;
}

void ResourceFormatLoaderGCL::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("gcsf");
	p_extensions->push_back("gclib");
}

bool ResourceFormatLoaderGCL::handles_type(const String &p_type) const {
	return p_type == "Script" || p_type == "GCLScript";
}

String ResourceFormatLoaderGCL::get_resource_type(const String &p_path) const {
	String ext = p_path.get_extension().to_lower();
	if (ext == "gcsf" || ext == "gclib") {
		return "GCLScript";
	}
	return String();
}

Error ResourceFormatSaverGCL::save(const String &p_path, const RES &p_resource, uint32_t p_flags) {
	Ref<GCLScript> script = p_resource;
	ERR_FAIL_COND_V(script.is_null(), ERR_INVALID_PARAMETER);

	FileAccess *file = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V(!file, ERR_CANT_OPEN);

	file->store_string(script->get_source_code());
	file->close();
	memdelete(file);
	return OK;
}

bool ResourceFormatSaverGCL::recognize(const RES &p_resource) const {
	return Object::cast_to<GCLScript>(*p_resource) != nullptr;
}

void ResourceFormatSaverGCL::get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const {
	Ref<GCLScript> script = p_resource;
	if (script.is_null()) {
		return;
	}
	if (!script->get_path().get_extension().empty()) {
		p_extensions->push_back(script->get_path().get_extension());
	}
	p_extensions->push_back("gcsf");
	p_extensions->push_back("gclib");
}

} // namespace gcl
