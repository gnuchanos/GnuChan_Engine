/**************************************************************************/
/*  print_string.h                                                        */
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

#ifndef PRINT_STRING_H
#define PRINT_STRING_H

#include "core/ustring.h"

extern void (*_print_func)(String);

typedef void (*PrintHandlerFunc)(void *, const String &p_string, bool p_error);

struct PrintHandlerList {
	PrintHandlerFunc printfunc;
	void *userdata;

	PrintHandlerList *next;

	PrintHandlerList() {
		printfunc = nullptr;
		next = nullptr;
		userdata = nullptr;
	}
};

void add_print_handler(PrintHandlerList *p_handler);
void remove_print_handler(PrintHandlerList *p_handler);

extern bool _print_line_enabled;
extern bool _print_error_enabled;
extern void print_line(String p_string);
extern void print_error(String p_string);
extern void print_verbose(String p_string);

#endif // PRINT_STRING_H
