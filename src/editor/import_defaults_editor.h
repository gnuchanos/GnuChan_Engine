/**************************************************************************/
/*  import_defaults_editor.h                                              */
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

#ifndef IMPORT_DEFAULTS_EDITOR_H
#define IMPORT_DEFAULTS_EDITOR_H

#include "core/undo_redo.h"
#include "editor/editor_data.h"
#include "editor/editor_plugin_settings.h"
#include "editor/editor_sectioned_inspector.h"
#include "editor_autoload_settings.h"
#include "scene/gui/center_container.h"
#include "scene/gui/option_button.h"

class ImportDefaultsEditorSettings;

class ImportDefaultsEditor : public VBoxContainer {
	GDCLASS(ImportDefaultsEditor, VBoxContainer)

	OptionButton *importers;
	Button *save_defaults;
	Button *reset_defaults;

	EditorInspector *inspector;

	ImportDefaultsEditorSettings *settings;

	void _update_importer();
	void _importer_selected(int p_index);

	void _reset();
	void _save();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void clear();

	ImportDefaultsEditor();
	~ImportDefaultsEditor();
};

#endif // IMPORT_DEFAULTS_EDITOR_H
