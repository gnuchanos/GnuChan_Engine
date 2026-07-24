/**************************************************************************/
/*  ip_address.h                                                          */
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

#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#include "core/ustring.h"

struct IP_Address {
private:
	union {
		uint8_t field8[16];
		uint16_t field16[8];
		uint32_t field32[4];
	};

	bool valid;
	bool wildcard;

protected:
	void _parse_ipv6(const String &p_string);
	void _parse_ipv4(const String &p_string, int p_start, uint8_t *p_ret);

public:
	//operator Variant() const;
	bool operator==(const IP_Address &p_ip) const {
		if (p_ip.valid != valid) {
			return false;
		}
		if (!valid) {
			return false;
		}
		for (int i = 0; i < 4; i++) {
			if (field32[i] != p_ip.field32[i]) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const IP_Address &p_ip) const {
		if (p_ip.valid != valid) {
			return true;
		}
		if (!valid) {
			return true;
		}
		for (int i = 0; i < 4; i++) {
			if (field32[i] != p_ip.field32[i]) {
				return true;
			}
		}
		return false;
	}

	void clear();
	bool is_wildcard() const { return wildcard; }
	bool is_valid() const { return valid; }
	bool is_ipv4() const;
	const uint8_t *get_ipv4() const;
	void set_ipv4(const uint8_t *p_ip);

	const uint8_t *get_ipv6() const;
	void set_ipv6(const uint8_t *p_buf);

	operator String() const;
	IP_Address(const String &p_string);
	IP_Address(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, bool is_v6 = false);
	IP_Address() { clear(); }
};

#endif // IP_ADDRESS_H
