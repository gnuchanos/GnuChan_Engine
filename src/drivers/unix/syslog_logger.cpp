/**************************************************************************/
/*  syslog_logger.cpp                                                     */
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

#ifdef UNIX_ENABLED

#include "syslog_logger.h"
#include "core/print_string.h"
#include <syslog.h>

void SyslogLogger::logv(const char *p_format, va_list p_list, bool p_err) {
	if (!should_log(p_err)) {
		return;
	}

	vsyslog(p_err ? LOG_ERR : LOG_INFO, p_format, p_list);
}

void SyslogLogger::print_error(const char *p_function, const char *p_file, int p_line, const char *p_code, const char *p_rationale, ErrorType p_type) {
	if (!should_log(true)) {
		return;
	}

	const char *err_type = "**ERROR**";
	switch (p_type) {
		case ERR_ERROR:
			err_type = "**ERROR**";
			break;
		case ERR_WARNING:
			err_type = "**WARNING**";
			break;
		case ERR_SCRIPT:
			err_type = "**SCRIPT ERROR**";
			break;
		case ERR_SHADER:
			err_type = "**SHADER ERROR**";
			break;
		default:
			ERR_PRINT("Unknown error type");
			break;
	}

	const char *err_details;
	if (p_rationale && *p_rationale) {
		err_details = p_rationale;
	} else {
		err_details = p_code;
	}

	syslog(p_type == ERR_WARNING ? LOG_WARNING : LOG_ERR, "%s: %s\n   At: %s:%i:%s() - %s", err_type, err_details, p_file, p_line, p_function, p_code);
}

SyslogLogger::~SyslogLogger() {
}

#endif
