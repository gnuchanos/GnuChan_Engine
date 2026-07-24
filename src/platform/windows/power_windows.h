/**************************************************************************/
/*  power_windows.h                                                       */
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

#ifndef POWER_WINDOWS_H
#define POWER_WINDOWS_H

#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/os/os.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class PowerWindows {
private:
	int nsecs_left;
	int percent_left;
	OS::PowerState power_state;

	bool GetPowerInfo_Windows();

public:
	PowerWindows();
	virtual ~PowerWindows();

	OS::PowerState get_power_state();
	int get_power_seconds_left();
	int get_power_percent_left();
};

#endif // POWER_WINDOWS_H
