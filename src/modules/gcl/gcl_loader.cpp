/**************************************************************************/
/*  gcl_loader.cpp                                                        */
/**************************************************************************/
/*  ResourceFormatLoader for GCL files:                                   */
/*    .gcsf   normal GCL script                                           */
/*    .gclib  modular GCL library                                         */
/*  Reads the file text and produces a GCLScript resource.                */
/**************************************************************************/

#include "gcl_loader.h"
#include "core/os/file_access.h"
#include "gcl_script.h"
#include "Lexer/lexer.h"
#include "core/io/resource_saver.h"

namespace gcl {

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
