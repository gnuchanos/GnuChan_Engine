/**************************************************************************/
/*  gd_mono_log.h                                                         */
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

#ifndef GD_MONO_LOG_H
#define GD_MONO_LOG_H

#include <mono/utils/mono-logger.h>

#include "core/typedefs.h"

#if !defined(JAVASCRIPT_ENABLED) && !defined(IPHONE_ENABLED)
// We have custom mono log callbacks for WASM and iOS
#define GD_MONO_LOG_ENABLED
#endif

#ifdef GD_MONO_LOG_ENABLED
#include "core/os/file_access.h"
#endif

class GDMonoLog {
#ifdef GD_MONO_LOG_ENABLED
	int log_level_id;

	FileAccess *log_file;
	String log_file_path;

	bool _try_create_logs_dir(const String &p_logs_dir);
	void _delete_old_log_files(const String &p_logs_dir);

	static void mono_log_callback(const char *log_domain, const char *log_level, const char *message, mono_bool fatal, void *user_data);
#endif

	static GDMonoLog *singleton;

public:
	_FORCE_INLINE_ static GDMonoLog *get_singleton() { return singleton; }

	void initialize();

	GDMonoLog();
	~GDMonoLog();
};

#endif // GD_MONO_LOG_H
