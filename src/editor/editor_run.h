/**************************************************************************/
/*  editor_run.h                                                          */
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

#ifndef EDITOR_RUN_H
#define EDITOR_RUN_H

#include "core/os/os.h"
#include "scene/main/node.h"
class EditorRun {
public:
	enum Status {

		STATUS_PLAY,
		STATUS_PAUSED,
		STATUS_STOP
	};

	OS::ProcessID pid;

private:
	bool debug_collisions;
	bool debug_navigation;
	bool debug_shader_fallbacks;
	Status status;
	String running_scene;

public:
	Status get_status() const;
	String get_running_scene() const;
	Error run(const String &p_scene, const String &p_custom_args, const List<String> &p_breakpoints, const bool &p_skip_breakpoints = false);
	void run_native_notify() { status = STATUS_PLAY; }
	void stop();

	OS::ProcessID get_pid() const { return pid; }

	void set_debug_collisions(bool p_debug);
	bool get_debug_collisions() const;

	void set_debug_navigation(bool p_debug);
	bool get_debug_navigation() const;

	void set_debug_shader_fallbacks(bool p_debug);
	bool get_debug_shader_fallbacks() const;

	EditorRun();
};

#endif // EDITOR_RUN_H
