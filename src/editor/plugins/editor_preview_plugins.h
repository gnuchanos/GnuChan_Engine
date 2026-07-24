/**************************************************************************/
/*  editor_preview_plugins.h                                              */
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

#ifndef EDITOR_PREVIEW_PLUGINS_H
#define EDITOR_PREVIEW_PLUGINS_H

#include "editor/editor_resource_preview.h"

#include "core/safe_refcount.h"

void post_process_preview(Ref<Image> p_image);

class EditorTexturePreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorTexturePreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorTexturePreviewPlugin();
};

class EditorImagePreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorImagePreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorImagePreviewPlugin();
};

class EditorBitmapPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorBitmapPreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorBitmapPreviewPlugin();
};

class EditorPackedScenePreviewPlugin : public EditorResourcePreviewGenerator {
public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;
	virtual Ref<Texture> generate_from_path(const String &p_path, const Size2 &p_size) const;

	EditorPackedScenePreviewPlugin();
};

class EditorMaterialPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorMaterialPreviewPlugin, EditorResourcePreviewGenerator);

	RID scenario;
	RID sphere;
	RID sphere_instance;
	RID viewport;
	RID viewport_texture;
	RID light;
	RID light_instance;
	RID light2;
	RID light_instance2;
	RID camera;
	mutable SafeFlag preview_done;

	void _preview_done(const Variant &p_udata);

protected:
	static void _bind_methods();

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorMaterialPreviewPlugin();
	~EditorMaterialPreviewPlugin();
};

class EditorScriptPreviewPlugin : public EditorResourcePreviewGenerator {
public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorScriptPreviewPlugin();
};

class EditorAudioStreamPreviewPlugin : public EditorResourcePreviewGenerator {
public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorAudioStreamPreviewPlugin();
};

class EditorMeshPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorMeshPreviewPlugin, EditorResourcePreviewGenerator);

	RID scenario;
	RID mesh_instance;
	RID viewport;
	RID viewport_texture;
	RID light;
	RID light_instance;
	RID light2;
	RID light_instance2;
	RID camera;
	mutable SafeFlag preview_done;

	void _preview_done(const Variant &p_udata);

protected:
	static void _bind_methods();

public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;

	EditorMeshPreviewPlugin();
	~EditorMeshPreviewPlugin();
};

class EditorFontPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorFontPreviewPlugin, EditorResourcePreviewGenerator);

	RID viewport;
	RID viewport_texture;
	RID canvas;
	RID canvas_item;
	mutable SafeFlag preview_done;

	void _preview_done(const Variant &p_udata);

protected:
	static void _bind_methods();

public:
	virtual bool handles(const String &p_type) const;
	virtual Ref<Texture> generate(const RES &p_from, const Size2 &p_size) const;
	virtual Ref<Texture> generate_from_path(const String &p_path, const Size2 &p_size) const;

	EditorFontPreviewPlugin();
	~EditorFontPreviewPlugin();
};

class EditorGradientPreviewPlugin : public EditorResourcePreviewGenerator {
	GDCLASS(EditorGradientPreviewPlugin, EditorResourcePreviewGenerator);

public:
	virtual bool handles(const String &p_type) const;
	virtual bool generate_small_preview_automatically() const;
	virtual Ref<Texture> generate(const Ref<Resource> &p_from, const Size2 &p_size) const;

	EditorGradientPreviewPlugin();
};

#endif // EDITOR_PREVIEW_PLUGINS_H
