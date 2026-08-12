/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*  Module entry: registers GCLScript in the engine ScriptServer and      */
/*  plugs in the .gcl resource loader/saver.                              */
/**************************************************************************/

#include "register_types.h"

#include "core/class_db.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/os/file_access.h"
#include "core/script_language.h"
#include "gcl_script.h"

/* ------------------------------------------------------------------ */
/*  Resource format handlers                                           */
/* ------------------------------------------------------------------ */

class ResourceFormatLoaderGCL : public ResourceFormatLoader {
public:
	RES load(const String &p_path, const String &p_original_path, Error *r_error, bool p_no_subresource_cache = false);
	void get_recognized_extensions(List<String> *p_extensions) const;
	bool handles_type(const String &p_type) const;
	String get_resource_type(const String &p_path) const;
};

RES ResourceFormatLoaderGCL::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_no_subresource_cache) {
	Error err;
	FileAccess *f = FileAccess::open(p_path, FileAccess::READ, &err);
	if (!f) {
		if (r_error) {
			*r_error = err;
		}
		return RES();
	}

	String source = f->get_as_utf8_string();
	memdelete(f);

	Ref<GCLScript> script = memnew(GCLScript);
	script->set_path(p_original_path, true);
	script->set_source_code(source);
	err = script->reload();
	if (r_error) {
		*r_error = err;
	}
	if (err != OK) {
		return RES();
	}
	return script;
}

void ResourceFormatLoaderGCL::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("gcl");
}

bool ResourceFormatLoaderGCL::handles_type(const String &p_type) const {
	return p_type == "Script" || p_type == "GCLScript";
}

String ResourceFormatLoaderGCL::get_resource_type(const String &p_path) const {
	if (p_path.get_extension().to_lower() == "gcl") {
		return "GCLScript";
	}
	return "";
}

class ResourceFormatSaverGCL : public ResourceFormatSaver {
public:
	Error save(const String &p_path, const RES &p_resource, uint32_t p_flags = 0);
	void get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const;
	bool recognize(const RES &p_resource) const;
};

Error ResourceFormatSaverGCL::save(const String &p_path, const RES &p_resource, uint32_t p_flags) {
	Ref<GCLScript> script = p_resource;
	ERR_FAIL_COND_V(script.is_null(), ERR_INVALID_PARAMETER);

	Error err;
	FileAccess *f = FileAccess::open(p_path, FileAccess::WRITE, &err);
	ERR_FAIL_COND_V_MSG(err, err, "Cannot save GCL file '" + p_path + "'.");

	f->store_string(script->get_source_code());
	f->close();
	memdelete(f);
	return OK;
}

void ResourceFormatSaverGCL::get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const {
	if (Object::cast_to<GCLScript>(*p_resource)) {
		p_extensions->push_back("gcl");
	}
}

bool ResourceFormatSaverGCL::recognize(const RES &p_resource) const {
	return Object::cast_to<GCLScript>(*p_resource) != nullptr;
}

/* Keep the loader/saver alive for the whole process lifetime.
 * Local Refs would be destroyed at the end of register_gcl_types() while
 * ResourceLoader/ResourceSaver still pointed at them, which caused dangling
 * objects and heap corruption at exit. */
static Ref<ResourceFormatLoaderGCL> gcl_resource_loader;
static Ref<ResourceFormatSaverGCL> gcl_resource_saver;

/* ------------------------------------------------------------------ */
/*  Registration                                                       */
/* ------------------------------------------------------------------ */

void register_gcl_types() {
	ClassDB::register_class<GCLScript>();

	GCLScriptLanguage *gcl_language = memnew(GCLScriptLanguage);
	ScriptServer::register_language(gcl_language);

	gcl_resource_loader.instance();
	ResourceLoader::add_resource_format_loader(gcl_resource_loader);

	gcl_resource_saver.instance();
	ResourceSaver::add_resource_format_saver(gcl_resource_saver);
}

void unregister_gcl_types() {
	ResourceLoader::remove_resource_format_loader(gcl_resource_loader);
	gcl_resource_loader.unref();

	ResourceSaver::remove_resource_format_saver(gcl_resource_saver);
	gcl_resource_saver.unref();

	GCLScriptLanguage *gcl_language = GCLScriptLanguage::get_singleton();
	if (gcl_language) {
		ScriptServer::unregister_language(gcl_language);
		memdelete(gcl_language);
	}
}
