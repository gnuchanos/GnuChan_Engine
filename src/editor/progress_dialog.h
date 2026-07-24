/**************************************************************************/
/*  progress_dialog.h                                                     */
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

#ifndef PROGRESS_DIALOG_H
#define PROGRESS_DIALOG_H

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/popup.h"
#include "scene/gui/progress_bar.h"

class BackgroundProgress : public HBoxContainer {
	GDCLASS(BackgroundProgress, HBoxContainer);

	_THREAD_SAFE_CLASS_

	struct Task {
		HBoxContainer *hb;
		ProgressBar *progress;
	};

	Map<String, Task> tasks;
	Map<String, int> updates;
	void _update();

protected:
	void _add_task(const String &p_task, const String &p_label, int p_steps);
	void _task_step(const String &p_task, int p_step = -1);
	void _end_task(const String &p_task);

	static void _bind_methods();

public:
	void add_task(const String &p_task, const String &p_label, int p_steps);
	void task_step(const String &p_task, int p_step = -1);
	void end_task(const String &p_task);

	BackgroundProgress() {}
};

class ProgressDialog : public Popup {
	GDCLASS(ProgressDialog, Popup);
	struct Task {
		String task;
		VBoxContainer *vb;
		ProgressBar *progress;
		Label *state;
		uint64_t last_progress_tick;
	};
	HBoxContainer *cancel_hb;
	Button *cancel;

	Map<String, Task> tasks;
	VBoxContainer *main;

	static ProgressDialog *singleton;
	void _popup();

	void _cancel_pressed();
	bool cancelled;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	static ProgressDialog *get_singleton() { return singleton; }
	void add_task(const String &p_task, const String &p_label, int p_steps, bool p_can_cancel = false);
	bool task_step(const String &p_task, const String &p_state, int p_step = -1, bool p_force_redraw = true);
	void end_task(const String &p_task);

	ProgressDialog();
};

#endif // PROGRESS_DIALOG_H
