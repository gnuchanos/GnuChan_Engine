/**************************************************************************/
/*  gd_mono_assembly.h                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHAN ENGINE                             */
/*                    https://github.com/gnuchanos/GnuChan_Engine         */
/**************************************************************************/
/* Copyright (C) 2025 GnuChan Engine contributors (see AUTHORS.md).      */
/*                                                                        */
/* This program is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program. If not, see <https://www.gnu.org/licenses/>.  */
/**************************************************************************/

#ifndef GD_MONO_ASSEMBLY_H
#define GD_MONO_ASSEMBLY_H

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "core/hash_map.h"
#include "core/map.h"
#include "core/ustring.h"
#include "gd_mono_utils.h"

class GDMonoAssembly {
	struct ClassKey {
		struct Hasher {
			static _FORCE_INLINE_ uint32_t hash(const ClassKey &p_key) {
				uint32_t hash = 0;

				GDMonoUtils::hash_combine(hash, p_key.namespace_name.hash());
				GDMonoUtils::hash_combine(hash, p_key.class_name.hash());

				return hash;
			}
		};

		_FORCE_INLINE_ bool operator==(const ClassKey &p_a) const {
			return p_a.class_name == class_name && p_a.namespace_name == namespace_name;
		}

		ClassKey() {}

		ClassKey(const StringName &p_namespace_name, const StringName &p_class_name) {
			namespace_name = p_namespace_name;
			class_name = p_class_name;
		}

		StringName namespace_name;
		StringName class_name;
	};

	String name;
	MonoImage *image;
	MonoAssembly *assembly;

#ifdef GD_MONO_HOT_RELOAD
	uint64_t modified_time;
#endif

	bool gdobject_class_cache_updated;
	Map<StringName, GDMonoClass *> gdobject_class_cache;

	HashMap<ClassKey, GDMonoClass *, ClassKey::Hasher> cached_classes;
	Map<MonoClass *, GDMonoClass *> cached_raw;

	static Vector<String> search_dirs;

	static void assembly_load_hook(MonoAssembly *assembly, void *user_data);
	static MonoAssembly *assembly_search_hook(MonoAssemblyName *aname, void *user_data);
	static MonoAssembly *assembly_refonly_search_hook(MonoAssemblyName *aname, void *user_data);
	static MonoAssembly *assembly_preload_hook(MonoAssemblyName *aname, char **assemblies_path, void *user_data);
	static MonoAssembly *assembly_refonly_preload_hook(MonoAssemblyName *aname, char **assemblies_path, void *user_data);

	static MonoAssembly *_search_hook(MonoAssemblyName *aname, void *user_data, bool refonly);
	static MonoAssembly *_preload_hook(MonoAssemblyName *aname, char **assemblies_path, void *user_data, bool refonly);

	static MonoAssembly *_real_load_assembly_from(const String &p_path, bool p_refonly, MonoAssemblyName *p_aname = nullptr);
	static MonoAssembly *_load_assembly_search(const String &p_name, MonoAssemblyName *p_aname, bool p_refonly, const Vector<String> &p_search_dirs);

	friend class GDMono;
	static void initialize();

public:
	void unload();

	_FORCE_INLINE_ MonoImage *get_image() const { return image; }
	_FORCE_INLINE_ MonoAssembly *get_assembly() const { return assembly; }
	_FORCE_INLINE_ String get_name() const { return name; }

#ifdef GD_MONO_HOT_RELOAD
	_FORCE_INLINE_ uint64_t get_modified_time() const { return modified_time; }
#endif

	String get_path() const;

	GDMonoClass *get_class(const StringName &p_namespace, const StringName &p_name);
	GDMonoClass *get_class(MonoClass *p_mono_class);

	GDMonoClass *get_object_derived_class(const StringName &p_class);

	static String find_assembly(const String &p_name);

	static void fill_search_dirs(Vector<String> &r_search_dirs, const String &p_custom_config = String(), const String &p_custom_bcl_dir = String());
	static const Vector<String> &get_default_search_dirs() { return search_dirs; }

	static GDMonoAssembly *load(const String &p_name, MonoAssemblyName *p_aname, bool p_refonly, const Vector<String> &p_search_dirs);
	static GDMonoAssembly *load_from(const String &p_name, const String &p_path, bool p_refonly);

	GDMonoAssembly(const String &p_name, MonoImage *p_image, MonoAssembly *p_assembly);
	~GDMonoAssembly();
};

#endif // GD_MONO_ASSEMBLY_H
