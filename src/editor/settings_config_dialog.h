/**************************************************************************/
/*  settings_config_dialog.h                                              */
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

#ifndef SETTINGS_CONFIG_DIALOG_H
#define SETTINGS_CONFIG_DIALOG_H

#include "editor/editor_sectioned_inspector.h"
#include "editor_inspector.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/tool_button.h"

class EditorSettingsDialog : public AcceptDialog {
	GDCLASS(EditorSettingsDialog, AcceptDialog);

	bool updating;

	TabContainer *tabs;
	Control *tab_general;
	Control *tab_shortcuts;

	LineEdit *search_box;
	LineEdit *shortcut_search_box;
	SectionedInspector *inspector;

	Timer *timer;

	UndoRedo *undo_redo;
	Tree *shortcuts;

	ConfirmationDialog *press_a_key;
	Label *press_a_key_label;
	Ref<InputEventKey> last_wait_for_key;
	String shortcut_configured;
	String shortcut_filter;

	virtual void cancel_pressed();
	virtual void ok_pressed();

	void _settings_changed();
	void _settings_property_edited(const String &p_name);
	void _settings_save();

	void _unhandled_input(const Ref<InputEvent> &p_event);
	void _notification(int p_what);
	void _update_icons();

	void _press_a_key_confirm();
	void _wait_for_key(const Ref<InputEvent> &p_event);

	void _tabs_tab_changed(int p_tab);
	void _focus_current_search_box();

	void _filter_shortcuts(const String &p_filter);

	void _update_shortcuts();
	void _shortcut_button_pressed(Object *p_item, int p_column, int p_idx);

	static void _undo_redo_callback(void *p_self, const String &p_name);

	Label *restart_label;
	TextureRect *restart_icon;
	PanelContainer *restart_container;
	ToolButton *restart_close_button;

	void _editor_restart_request();
	void _editor_restart();
	void _editor_restart_close();

protected:
	static void _bind_methods();

public:
	void popup_edit_settings();

	EditorSettingsDialog();
	~EditorSettingsDialog();
};

#endif // SETTINGS_CONFIG_DIALOG_H
