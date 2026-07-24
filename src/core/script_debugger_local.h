/**************************************************************************/
/*  script_debugger_local.h                                               */
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

#ifndef SCRIPT_DEBUGGER_LOCAL_H
#define SCRIPT_DEBUGGER_LOCAL_H

#include "core/list.h"
#include "core/script_language.h"

class ScriptDebuggerLocal : public ScriptDebugger {
	bool profiling;
	float frame_time, process_time, physics_time, physics_frame_time;
	uint64_t idle_accum;
	String target_function;
	Map<String, String> options;

	Vector<ScriptLanguage::ProfilingInfo> pinfo;

	Pair<String, int> to_breakpoint(const String &p_line);
	void print_variables(const List<String> &names, const List<Variant> &values, const String &variable_prefix);

public:
	void debug(ScriptLanguage *p_script, bool p_can_continue, bool p_is_error_breakpoint);
	virtual void send_message(const String &p_message, const Array &p_args);
	virtual void send_error(const String &p_func, const String &p_file, int p_line, const String &p_err, const String &p_descr, ErrorHandlerType p_type, const Vector<ScriptLanguage::StackInfo> &p_stack_info);

	virtual bool is_profiling() const { return profiling; }
	virtual void add_profiling_frame_data(const StringName &p_name, const Array &p_data) {}

	virtual void idle_poll();

	virtual void profiling_start();
	virtual void profiling_end();
	virtual void profiling_set_frame_times(float p_frame_time, float p_process_time, float p_physics_time, float p_physics_frame_time);

	ScriptDebuggerLocal();
};

#endif // SCRIPT_DEBUGGER_LOCAL_H
