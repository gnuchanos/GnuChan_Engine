/**************************************************************************/
/*  websocket_macros.h                                                    */
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

#ifndef WEBSOCKET_MACROS_H
#define WEBSOCKET_MACROS_H

#define WSC_IN_BUF PNAME("network/limits/websocket_client/max_in_buffer_kb")
#define WSC_IN_PKT PNAME("network/limits/websocket_client/max_in_packets")
#define WSC_OUT_BUF PNAME("network/limits/websocket_client/max_out_buffer_kb")
#define WSC_OUT_PKT PNAME("network/limits/websocket_client/max_out_packets")

#define WSS_IN_BUF PNAME("network/limits/websocket_server/max_in_buffer_kb")
#define WSS_IN_PKT PNAME("network/limits/websocket_server/max_in_packets")
#define WSS_OUT_BUF PNAME("network/limits/websocket_server/max_out_buffer_kb")
#define WSS_OUT_PKT PNAME("network/limits/websocket_server/max_out_packets")

#define GDCICLASS(CNAME)              \
public:                               \
	static CNAME *(*_create)();       \
                                      \
	static Ref<CNAME> create_ref() {  \
		if (!_create)                 \
			return Ref<CNAME>();      \
		return Ref<CNAME>(_create()); \
	}                                 \
                                      \
	static CNAME *create() {          \
		if (!_create)                 \
			return NULL;              \
		return _create();             \
	}                                 \
                                      \
protected:

#define GDCINULL(CNAME) \
	CNAME *(*CNAME::_create)() = NULL;

#define GDCIIMPL(IMPNAME, CNAME)                                      \
public:                                                               \
	static CNAME *_create() { return memnew(IMPNAME); }               \
	static void make_default() { CNAME::_create = IMPNAME::_create; } \
                                                                      \
protected:

#endif // WEBSOCKET_MACROS_H
