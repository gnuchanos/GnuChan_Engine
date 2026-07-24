/**************************************************************************/
/*  gdscript_language_server.cpp                                          */
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

#include "gdscript_language_server.h"

#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor/editor_log.h"
#include "editor/editor_node.h"

int GDScriptLanguageServer::port_override = -1;

GDScriptLanguageServer::GDScriptLanguageServer() {
	thread_running = false;
	started = false;
	use_thread = false;
	host = "127.0.0.1";
	port = 6008;

	_EDITOR_DEF("network/language_server/remote_host", host);
	_EDITOR_DEF("network/language_server/remote_port", port);
	_EDITOR_DEF("network/language_server/enable_smart_resolve", true);
	_EDITOR_DEF("network/language_server/show_native_symbols_in_editor", false);
	_EDITOR_DEF("network/language_server/use_thread", use_thread);
	_EDITOR_DEF("network/language_server/poll_limit_usec", poll_limit_usec);
}

void GDScriptLanguageServer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
			start();
			break;
		case NOTIFICATION_EXIT_TREE:
			stop();
			break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (started && !use_thread) {
				protocol.poll(poll_limit_usec);
			}
		} break;
		case EditorSettings::NOTIFICATION_EDITOR_SETTINGS_CHANGED: {
			String host = String(_EDITOR_GET("network/language_server/remote_host"));
			int port = (GDScriptLanguageServer::port_override > -1) ? GDScriptLanguageServer::port_override : (int)_EDITOR_GET("network/language_server/remote_port");
			bool use_thread = (bool)_EDITOR_GET("network/language_server/use_thread");
			int remote_poll_limit = (int)_EDITOR_GET("network/language_server/poll_limit_usec");
			if (host != this->host || port != this->port || use_thread != this->use_thread || remote_poll_limit != poll_limit_usec) {
				this->stop();
				this->start();
			}
		} break;
	}
}

void GDScriptLanguageServer::thread_main(void *p_userdata) {
	GDScriptLanguageServer *self = static_cast<GDScriptLanguageServer *>(p_userdata);
	while (self->thread_running) {
		// Poll 20 times per second
		self->protocol.poll(self->poll_limit_usec);
		OS::get_singleton()->delay_usec(50000);
	}
}

void GDScriptLanguageServer::start() {
	host = String(_EDITOR_GET("network/language_server/remote_host"));
	port = (GDScriptLanguageServer::port_override > -1) ? GDScriptLanguageServer::port_override : (int)_EDITOR_GET("network/language_server/remote_port");
	use_thread = (bool)_EDITOR_GET("network/language_server/use_thread");
	poll_limit_usec = (int)_EDITOR_GET("network/language_server/poll_limit_usec");
	if (protocol.start(port, IP_Address(host)) == OK) {
		EditorNode::get_log()->add_message("--- GDScript language server started on port " + itos(port) + " ---", EditorLog::MSG_TYPE_EDITOR);
		if (use_thread) {
			thread_running = true;
			thread.start(GDScriptLanguageServer::thread_main, this);
		}
		set_process_internal(!use_thread);
		started = true;
	}
}

void GDScriptLanguageServer::stop() {
	if (use_thread) {
		ERR_FAIL_COND(!thread.is_started());
		thread_running = false;
		thread.wait_to_finish();
	}
	protocol.stop();
	started = false;
	EditorNode::get_log()->add_message("--- GDScript language server stopped ---", EditorLog::MSG_TYPE_EDITOR);
}

void register_lsp_types() {
	ClassDB::register_class<GDScriptLanguageProtocol>();
	ClassDB::register_class<GDScriptTextDocument>();
	ClassDB::register_class<GDScriptWorkspace>();
}
