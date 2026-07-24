/**************************************************************************/
/*  gdscript_language_protocol.h                                          */
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

#ifndef GDSCRIPT_LANGUAGE_PROTOCOL_H
#define GDSCRIPT_LANGUAGE_PROTOCOL_H

#include "core/io/stream_peer.h"
#include "core/io/stream_peer_tcp.h"
#include "core/io/tcp_server.h"
#include "gdscript_text_document.h"
#include "gdscript_workspace.h"
#include "lsp.hpp"
#include "modules/jsonrpc/jsonrpc.h"

#define LSP_MAX_BUFFER_SIZE 4194304
#define LSP_MAX_CLIENTS 8

class GDScriptLanguageProtocol : public JSONRPC {
	GDCLASS(GDScriptLanguageProtocol, JSONRPC)

private:
	struct LSPeer : Reference {
		Ref<StreamPeerTCP> connection;

		uint8_t req_buf[LSP_MAX_BUFFER_SIZE];
		int req_pos = 0;
		bool has_header = false;
		bool has_content = false;
		int content_length = 0;
		Vector<CharString> res_queue;
		int res_sent = 0;

		Error handle_data();
		Error send_data();
	};

	enum LSPErrorCode {
		RequestCancelled = -32800,
		ContentModified = -32801,
	};

	static GDScriptLanguageProtocol *singleton;

	HashMap<int, Ref<LSPeer>> clients;
	Ref<TCP_Server> server;
	int latest_client_id = 0;
	int next_client_id = 0;
	int next_server_id = 0;

	Ref<GDScriptTextDocument> text_document;
	Ref<GDScriptWorkspace> workspace;

	Error on_client_connected();
	void on_client_disconnected(const int &p_client_id);

	String process_message(const String &p_text);
	String format_output(const String &p_text);

	bool _initialized = false;

protected:
	static void _bind_methods();

	Dictionary initialize(const Dictionary &p_params);
	void initialized(const Variant &p_params);

public:
	_FORCE_INLINE_ static GDScriptLanguageProtocol *get_singleton() { return singleton; }
	_FORCE_INLINE_ Ref<GDScriptWorkspace> get_workspace() { return workspace; }
	_FORCE_INLINE_ Ref<GDScriptTextDocument> get_text_document() { return text_document; }
	_FORCE_INLINE_ bool is_initialized() const { return _initialized; }

	void poll(int p_limit_usec);
	Error start(int p_port, const IP_Address &p_bind_ip);
	void stop();

	void notify_client(const String &p_method, const Variant &p_params = Variant(), int p_client_id = -1);
	void request_client(const String &p_method, const Variant &p_params = Variant(), int p_client_id = -1);

	bool is_smart_resolve_enabled() const;
	bool is_goto_native_symbols_enabled() const;

	GDScriptLanguageProtocol();
};

#endif // GDSCRIPT_LANGUAGE_PROTOCOL_H
