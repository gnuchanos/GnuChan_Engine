/**************************************************************************/
/*  run_settings_dialog.h                                                 */
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

#ifndef RUN_SETTINGS_DIALOG_H
#define RUN_SETTINGS_DIALOG_H

#include "scene/gui/check_button.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/file_dialog.h"
#include "scene/gui/line_edit.h"

class RunSettingsDialog : public AcceptDialog {
	GDCLASS(RunSettingsDialog, AcceptDialog);

public:
	enum RunMode {
		RUN_LOCAL_SCENE,
		RUN_MAIN_SCENE,
	};

private:
	OptionButton *run_mode;
	LineEdit *arguments;

	void _run_mode_changed(int idx);

protected:
	static void _bind_methods();

public:
	int get_run_mode() const;
	void set_run_mode(int p_run_mode);

	void set_custom_arguments(const String &p_arguments);
	String get_custom_arguments() const;

	void popup_run_settings();

	RunSettingsDialog();
};

#endif // RUN_SETTINGS_DIALOG_H
