/**************************************************************************/
/*  logger.h                                                              */
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

#ifndef LOGGER_H
#define LOGGER_H

#include "core/os/file_access.h"
#include "core/ustring.h"
#include "core/vector.h"

#include <stdarg.h>

class Logger {
protected:
	bool should_log(bool p_err);

	static bool _flush_stdout_on_print;

public:
	enum ErrorType {
		ERR_ERROR,
		ERR_WARNING,
		ERR_SCRIPT,
		ERR_SHADER
	};

	static void set_flush_stdout_on_print(bool value);

	virtual void logv(const char *p_format, va_list p_list, bool p_err) _PRINTF_FORMAT_ATTRIBUTE_2_0 = 0;
	virtual void log_error(const char *p_function, const char *p_file, int p_line, const char *p_code, const char *p_rationale, ErrorType p_type = ERR_ERROR);

	void logf(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_2_3;
	void logf_error(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_2_3;

	virtual ~Logger();
};

/**
 * Writes messages to stdout/stderr.
 */
class StdLogger : public Logger {
public:
	virtual void logv(const char *p_format, va_list p_list, bool p_err) _PRINTF_FORMAT_ATTRIBUTE_2_0;
	virtual ~StdLogger();
};

/**
 * Writes messages to the specified file. If the file already exists, creates a copy (backup)
 * of it with timestamp appended to the file name. Maximum number of backups is configurable.
 * When maximum is reached, the oldest backups are erased. With the maximum being equal to 1,
 * it acts as a simple file logger.
 */
class RotatedFileLogger : public Logger {
	String base_path;
	int max_files;

	FileAccess *file;

	void close_file();
	void clear_old_backups();
	void rotate_file();

public:
	RotatedFileLogger(const String &p_base_path, int p_max_files = 10);

	virtual void logv(const char *p_format, va_list p_list, bool p_err) _PRINTF_FORMAT_ATTRIBUTE_2_0;

	virtual ~RotatedFileLogger();
};

class CompositeLogger : public Logger {
	Vector<Logger *> loggers;

public:
	CompositeLogger(Vector<Logger *> p_loggers);

	virtual void logv(const char *p_format, va_list p_list, bool p_err) _PRINTF_FORMAT_ATTRIBUTE_2_0;
	virtual void log_error(const char *p_function, const char *p_file, int p_line, const char *p_code, const char *p_rationale, ErrorType p_type = ERR_ERROR);

	void add_logger(Logger *p_logger);

	virtual ~CompositeLogger();
};

#endif // LOGGER_H
