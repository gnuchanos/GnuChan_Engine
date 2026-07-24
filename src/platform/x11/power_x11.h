/**************************************************************************/
/*  power_x11.h                                                           */
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

#ifndef POWER_X11_H
#define POWER_X11_H

#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/os/os.h"

class PowerX11 {
private:
	int nsecs_left;
	int percent_left;
	OS::PowerState power_state;

	FileAccessRef open_power_file(const char *base, const char *node, const char *key);
	bool read_power_file(const char *base, const char *node, const char *key, char *buf, size_t buflen);
	bool make_proc_acpi_key_val(char **_ptr, char **_key, char **_val);
	void check_proc_acpi_battery(const char *node, bool *have_battery, bool *charging);
	void check_proc_acpi_ac_adapter(const char *node, bool *have_ac);
	bool GetPowerInfo_Linux_proc_acpi();
	bool next_string(char **_ptr, char **_str);
	bool int_string(char *str, int *val);
	bool GetPowerInfo_Linux_proc_apm();
	bool GetPowerInfo_Linux_sys_class_power_supply();
	bool UpdatePowerInfo();

public:
	PowerX11();
	virtual ~PowerX11();

	OS::PowerState get_power_state();
	int get_power_seconds_left();
	int get_power_percent_left();
};

#endif // POWER_X11_H
