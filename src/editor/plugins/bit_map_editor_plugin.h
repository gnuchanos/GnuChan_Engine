/**************************************************************************/
/*  bit_map_editor_plugin.h                                               */
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

#ifndef BIT_MAP_EDITOR_PLUGIN_H
#define BIT_MAP_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"
#include "scene/resources/bit_map.h"

class BitMapEditor : public VBoxContainer {
	GDCLASS(BitMapEditor, VBoxContainer);

	TextureRect *texture_rect = nullptr;
	Label *size_label = nullptr;

public:
	void setup(const Ref<BitMap> &p_bitmap);

	BitMapEditor();
};

class EditorInspectorPluginBitMap : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginBitMap, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object);
	virtual void parse_begin(Object *p_object);
};

class BitMapEditorPlugin : public EditorPlugin {
	GDCLASS(BitMapEditorPlugin, EditorPlugin);

public:
	BitMapEditorPlugin(EditorNode *p_editor);
};

#endif // BIT_MAP_EDITOR_PLUGIN_H
