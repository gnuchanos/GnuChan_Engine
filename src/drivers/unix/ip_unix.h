/**************************************************************************/
/*  ip_unix.h                                                             */
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

#ifndef IP_UNIX_H
#define IP_UNIX_H

#include "core/io/ip.h"

#if defined(UNIX_ENABLED) || defined(WINDOWS_ENABLED)

class IP_Unix : public IP {
	GDCLASS(IP_Unix, IP);

	virtual void _resolve_hostname(List<IP_Address> &r_addresses, const String &p_hostname, Type p_type = TYPE_ANY) const;

	static IP *_create_unix();

public:
	virtual void get_local_interfaces(Map<String, Interface_Info> *r_interfaces) const;

	static void make_default();
	IP_Unix();
};

#endif

#endif // IP_UNIX_H
