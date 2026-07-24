/**************************************************************************/
/*  plugin_config_dialog.h                                                */
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

#ifndef PLUGIN_CONFIG_DIALOG_H
#define PLUGIN_CONFIG_DIALOG_H

#include "scene/gui/check_box.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/text_edit.h"

class PluginConfigDialog : public ConfirmationDialog {
	GDCLASS(PluginConfigDialog, ConfirmationDialog);

	LineEdit *name_edit;
	LineEdit *subfolder_edit;
	TextEdit *desc_edit;
	LineEdit *author_edit;
	LineEdit *version_edit;
	OptionButton *script_option_edit;
	LineEdit *script_edit;
	CheckBox *active_edit;

	bool _edit_mode;

	void _clear_fields();
	void _on_confirmed();
	void _on_cancelled();
	void _on_required_text_changed(const String &p_text);

protected:
	virtual void _notification(int p_what);
	static void _bind_methods();

public:
	void config(const String &p_config_path);

	PluginConfigDialog();
	~PluginConfigDialog();
};

#endif // PLUGIN_CONFIG_DIALOG_H
