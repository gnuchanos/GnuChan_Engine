/**************************************************************************/
/*  gdnative_library_singleton_editor.h                                   */
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

#ifndef GDNATIVE_LIBRARY_SINGLETON_EDITOR_H
#define GDNATIVE_LIBRARY_SINGLETON_EDITOR_H

#ifdef TOOLS_ENABLED
#include "editor/editor_file_system.h"
#include "editor/project_settings_editor.h"

class GDNativeLibrarySingletonEditor : public VBoxContainer {
	GDCLASS(GDNativeLibrarySingletonEditor, VBoxContainer);

private:
	Tree *libraries;
	UndoRedo *undo_redo;

	bool updating;

	static Set<String> _find_singletons_recursive(EditorFileSystemDirectory *p_dir);

protected:
	void _notification(int p_what);
	static void _bind_methods();

	void _discover_singletons();
	void _item_edited();
	void _update_libraries();

public:
	GDNativeLibrarySingletonEditor();
};

#endif

#endif // GDNATIVE_LIBRARY_SINGLETON_EDITOR_H
