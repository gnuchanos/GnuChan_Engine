/**************************************************************************/
/*  ip.h                                                                  */
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

#ifndef IP_H
#define IP_H

#include "core/io/ip_address.h"
#include "core/os/os.h"

struct _IP_ResolverPrivate;

class IP : public Object {
	GDCLASS(IP, Object);
	OBJ_CATEGORY("Networking");

public:
	enum ResolverStatus {

		RESOLVER_STATUS_NONE,
		RESOLVER_STATUS_WAITING,
		RESOLVER_STATUS_DONE,
		RESOLVER_STATUS_ERROR,
	};

	enum Type {

		TYPE_NONE = 0,
		TYPE_IPV4 = 1,
		TYPE_IPV6 = 2,
		TYPE_ANY = 3,
	};

	enum {
		RESOLVER_MAX_QUERIES = 256,
		RESOLVER_INVALID_ID = -1
	};

	typedef int ResolverID;

private:
	_IP_ResolverPrivate *resolver;

protected:
	static IP *singleton;
	static void _bind_methods();

	Array _get_local_addresses() const;
	Array _get_local_interfaces() const;

	static IP *(*_create)();

public:
	struct Interface_Info {
		String name;
		String name_friendly;
		String index;
		List<IP_Address> ip_addresses;
	};

	IP_Address resolve_hostname(const String &p_hostname, Type p_type = TYPE_ANY);
	Array resolve_hostname_addresses(const String &p_hostname, Type p_type = TYPE_ANY);
	// async resolver hostname
	ResolverID resolve_hostname_queue_item(const String &p_hostname, Type p_type = TYPE_ANY);
	ResolverStatus get_resolve_item_status(ResolverID p_id) const;
	IP_Address get_resolve_item_address(ResolverID p_id) const;
	virtual void get_local_addresses(List<IP_Address> *r_addresses) const;

	virtual void _resolve_hostname(List<IP_Address> &r_addresses, const String &p_hostname, Type p_type = TYPE_ANY) const = 0;
	Array get_resolve_item_addresses(ResolverID p_id) const;
	virtual void get_local_interfaces(Map<String, Interface_Info> *r_interfaces) const = 0;
	void erase_resolve_item(ResolverID p_id);

	void clear_cache(const String &p_hostname = "");

	static IP *get_singleton();

	static IP *create();

	IP();
	~IP();
};

VARIANT_ENUM_CAST(IP::Type);

#endif // IP_H
