/**************************************************************************/
/*  print_string.cpp                                                      */
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

#include "print_string.h"

#include "core/os/os.h"

#include <stdio.h>

static PrintHandlerList *print_handler_list = nullptr;
bool _print_line_enabled = true;
bool _print_error_enabled = true;

void add_print_handler(PrintHandlerList *p_handler) {
	_global_lock();
	p_handler->next = print_handler_list;
	print_handler_list = p_handler;
	_global_unlock();
}

void remove_print_handler(PrintHandlerList *p_handler) {
	_global_lock();

	PrintHandlerList *prev = nullptr;
	PrintHandlerList *l = print_handler_list;

	while (l) {
		if (l == p_handler) {
			if (prev) {
				prev->next = l->next;
			} else {
				print_handler_list = l->next;
			}
			break;
		}
		prev = l;
		l = l->next;
	}
	//OS::get_singleton()->print("print handler list is %p\n",print_handler_list);

	_global_unlock();
	ERR_FAIL_COND(l == nullptr);
}

void print_line(String p_string) {
	if (!_print_line_enabled) {
		return;
	}

	OS::get_singleton()->print("%s\n", p_string.utf8().get_data());

	_global_lock();
	PrintHandlerList *l = print_handler_list;
	while (l) {
		l->printfunc(l->userdata, p_string, false);
		l = l->next;
	}

	_global_unlock();
}

void print_error(String p_string) {
	if (!_print_error_enabled) {
		return;
	}

	OS::get_singleton()->printerr("%s\n", p_string.utf8().get_data());

	_global_lock();
	PrintHandlerList *l = print_handler_list;
	while (l) {
		l->printfunc(l->userdata, p_string, true);
		l = l->next;
	}

	_global_unlock();
}

void print_verbose(String p_string) {
	if (OS::get_singleton()->is_stdout_verbose()) {
		print_line(p_string);
	}
}
