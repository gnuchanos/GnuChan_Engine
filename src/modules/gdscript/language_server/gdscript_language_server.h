/**************************************************************************/
/*  gdscript_language_server.h                                            */
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

#ifndef GDSCRIPT_LANGUAGE_SERVER_H
#define GDSCRIPT_LANGUAGE_SERVER_H

#include "../gdscript_parser.h"
#include "editor/editor_plugin.h"
#include "gdscript_language_protocol.h"

class GDScriptLanguageServer : public EditorPlugin {
	GDCLASS(GDScriptLanguageServer, EditorPlugin);

	GDScriptLanguageProtocol protocol;

	Thread thread;
	bool thread_running;
	bool started;
	bool use_thread;
	String host;
	int port;
	int poll_limit_usec = 100000;
	static void thread_main(void *p_userdata);

private:
	void _notification(int p_what);

public:
	static int port_override;
	GDScriptLanguageServer();
	void start();
	void stop();
};

void register_lsp_types();

#endif // GDSCRIPT_LANGUAGE_SERVER_H
