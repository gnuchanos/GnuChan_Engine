/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "core/error_macros.h"
#include "core/project_settings.h"

#include "websocket_client.h"
#include "websocket_server.h"

#ifdef JAVASCRIPT_ENABLED
#include "emscripten.h"
#include "emws_client.h"
#include "emws_peer.h"
#else
#include "wsl_client.h"
#include "wsl_server.h"
#endif

void register_websocket_types() {
#define _SET_HINT(NAME, _VAL_, _MAX_) \
	GLOBAL_DEF(NAME, _VAL_);          \
	ProjectSettings::get_singleton()->set_custom_property_info(NAME, PropertyInfo(Variant::INT, NAME, PROPERTY_HINT_RANGE, "2," #_MAX_ ",1,or_greater"));

	// Client buffers project settings
	_SET_HINT(WSC_IN_BUF, 64, 4096);
	_SET_HINT(WSC_IN_PKT, 1024, 16384);
	_SET_HINT(WSC_OUT_BUF, 64, 4096);
	_SET_HINT(WSC_OUT_PKT, 1024, 16384);

	// Server buffers project settings
	_SET_HINT(WSS_IN_BUF, 64, 4096);
	_SET_HINT(WSS_IN_PKT, 1024, 16384);
	_SET_HINT(WSS_OUT_BUF, 64, 4096);
	_SET_HINT(WSS_OUT_PKT, 1024, 16384);

#ifdef JAVASCRIPT_ENABLED
	EMWSPeer::make_default();
	EMWSClient::make_default();
#else
	WSLPeer::make_default();
	WSLClient::make_default();
	WSLServer::make_default();
#endif

	ClassDB::register_virtual_class<WebSocketMultiplayerPeer>();
	ClassDB::register_custom_instance_class<WebSocketServer>();
	ClassDB::register_custom_instance_class<WebSocketClient>();
	ClassDB::register_custom_instance_class<WebSocketPeer>();
}

void unregister_websocket_types() {}
