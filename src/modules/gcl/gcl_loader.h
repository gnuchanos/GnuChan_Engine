/**************************************************************************/
/*  gcl_loader.h                                                          */
/**************************************************************************/
/*  ResourceFormatLoader for GCL files:                                   */
/*    .gcsf   normal GCL script                                           */
/*    .gclib  modular GCL library                                         */
/**************************************************************************/

#ifndef GCL_LOADER_H
#define GCL_LOADER_H

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"

namespace gcl {

class ResourceFormatLoaderGCL : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderGCL, ResourceFormatLoader);

public:
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatSaverGCL : public ResourceFormatSaver {
	GDCLASS(ResourceFormatSaverGCL, ResourceFormatSaver);

public:
	virtual Error save(const String &p_path, const RES &p_resource, uint32_t p_flags = 0);
	virtual bool recognize(const RES &p_resource) const;
	virtual void get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const;
};

} // namespace gcl

#endif // GCL_LOADER_H
