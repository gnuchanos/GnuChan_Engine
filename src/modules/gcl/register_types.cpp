#include "register_types.h"
#include "core/class_db.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/object.h"
#include "core/register_core_types.h"
#include "core/script_language.h"
#include "gcl_loader.h"
#include "gcl_script.h"

#ifdef TOOLS_ENABLED
#include "Highlight/gcl_syntax_highlighter.h"
#endif

static gcl::GCLScriptLanguage *gcl_language = nullptr;
static Ref<gcl::ResourceFormatLoaderGCL> gcl_loader;
static Ref<gcl::ResourceFormatSaverGCL> gcl_saver;

void register_gcl_types() {
	/* Required: without ClassDB registration, Object::cast_to<Script> fails
	 * and Object::set_script throws "Invalid parameter, it should be a
	 * reference to a valid script (or null)." (core/object.cpp:1011). */
	ClassDB::register_class<gcl::GCLScript>();

	gcl_language = memnew(gcl::GCLScriptLanguage);
	ScriptServer::register_language(gcl_language);

	gcl_loader.instance();
	ResourceLoader::add_resource_format_loader(gcl_loader);

	gcl_saver.instance();
	ResourceSaver::add_resource_format_saver(gcl_saver);

#ifdef TOOLS_ENABLED
	/* Editor icin GCL syntax highlighter (printf/spec/deger renkleri). */
	gcl::register_gcl_syntax_highlighter();
#endif
}

void unregister_gcl_types() {
	if (gcl_saver.is_valid()) {
		ResourceSaver::remove_resource_format_saver(gcl_saver);
		gcl_saver = Ref<gcl::ResourceFormatSaverGCL>();
	}
	if (gcl_loader.is_valid()) {
		ResourceLoader::remove_resource_format_loader(gcl_loader);
		gcl_loader = Ref<gcl::ResourceFormatLoaderGCL>();
	}
	if (gcl_language) {
		ScriptServer::unregister_language(gcl_language);
		memdelete(gcl_language);
		gcl_language = nullptr;
	}
}
