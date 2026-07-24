/**************************************************************************/
/*  main.h                                                                */
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

#ifndef MAIN_H
#define MAIN_H

#include "core/error_list.h"
#include "core/os/thread.h"
#include "core/typedefs.h"

template <class T>
class Vector;

class Main {
	static void print_help(const char *p_binary);
	static uint64_t last_ticks;
	static uint32_t hide_print_fps_attempts;
	static uint32_t frames;
	static uint32_t frame;
	static bool force_redraw_requested;
	static int iterating;
	static bool agile_input_event_flushing;

public:
	static bool is_project_manager();
#ifdef TOOLS_ENABLED
	enum CLIScope {
		CLI_SCOPE_TOOL, // Editor and project manager.
		CLI_SCOPE_PROJECT,
	};
	static const Vector<String> &get_forwardable_cli_arguments(CLIScope p_scope);
#endif

	static Error setup(const char *execpath, int argc, char *argv[], bool p_second_phase = true);
	static Error setup2(Thread::ID p_main_tid_override = 0);
	static bool start();

	static bool iteration();
	static void force_redraw();

	static bool is_iterating();

	static void cleanup(bool p_force = false);
};

#endif // MAIN_H
