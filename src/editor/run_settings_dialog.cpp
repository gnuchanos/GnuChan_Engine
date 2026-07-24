/**************************************************************************/
/*  run_settings_dialog.cpp                                               */
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

#include "run_settings_dialog.h"

void RunSettingsDialog::popup_run_settings() {
	popup_centered(Size2(300, 150));
}

void RunSettingsDialog::set_custom_arguments(const String &p_arguments) {
	arguments->set_text(p_arguments);
}
String RunSettingsDialog::get_custom_arguments() const {
	return arguments->get_text();
}

void RunSettingsDialog::_bind_methods() {
	ClassDB::bind_method("_run_mode_changed", &RunSettingsDialog::_run_mode_changed);
	//ClassDB::bind_method("_browse_selected_file",&RunSettingsDialog::_browse_selected_file);
}

void RunSettingsDialog::_run_mode_changed(int idx) {
	if (idx == 0) {
		arguments->set_editable(false);
	} else {
		arguments->set_editable(true);
	}
}

int RunSettingsDialog::get_run_mode() const {
	return run_mode->get_selected();
}

void RunSettingsDialog::set_run_mode(int p_run_mode) {
	run_mode->select(p_run_mode);
	arguments->set_editable(p_run_mode);
}

RunSettingsDialog::RunSettingsDialog() {
	/* SNAP DIALOG */

	VBoxContainer *vbc = memnew(VBoxContainer);
	add_child(vbc);
	//set_child_rect(vbc);

	run_mode = memnew(OptionButton);
	vbc->add_margin_child(TTR("Run Mode:"), run_mode);
	run_mode->add_item(TTR("Current Scene"));
	run_mode->add_item(TTR("Main Scene"));
	run_mode->connect("item_selected", this, "_run_mode_changed");
	arguments = memnew(LineEdit);
	vbc->add_margin_child(TTR("Main Scene Arguments:"), arguments);
	arguments->set_editable(false);

	get_ok()->set_text(TTR("Close"));

	set_title(TTR("Scene Run Settings"));
}
