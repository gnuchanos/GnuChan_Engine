/**************************************************************************/
/*  dtls_server.cpp                                                       */
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

#include "dtls_server.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"

DTLSServer *(*DTLSServer::_create)() = nullptr;
bool DTLSServer::available = false;

DTLSServer *DTLSServer::create() {
	if (_create) {
		return _create();
	}
	return nullptr;
}

bool DTLSServer::is_available() {
	return available;
}

void DTLSServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("setup", "key", "certificate", "chain"), &DTLSServer::setup, DEFVAL(Ref<X509Certificate>()));
	ClassDB::bind_method(D_METHOD("take_connection", "udp_peer"), &DTLSServer::take_connection);
}

DTLSServer::DTLSServer() {
}
