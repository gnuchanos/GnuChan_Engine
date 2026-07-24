/**************************************************************************/
/*  editor_file_server.h                                                  */
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

#ifndef EDITOR_FILE_SERVER_H
#define EDITOR_FILE_SERVER_H

#include "core/io/file_access_network.h"
#include "core/io/packet_peer.h"
#include "core/io/tcp_server.h"
#include "core/object.h"
#include "core/os/thread.h"

class EditorFileServer : public Object {
	GDCLASS(EditorFileServer, Object);

	enum Command {
		CMD_NONE,
		CMD_ACTIVATE,
		CMD_STOP,
	};

	struct ClientData {
		Thread *thread;
		Ref<StreamPeerTCP> connection;
		Map<int, FileAccess *> files;
		EditorFileServer *efs;
		bool quit;
	};

	Ref<TCP_Server> server;
	Set<Thread *> to_wait;

	static void _close_client(ClientData *cd);
	static void _subthread_start(void *s);

	Mutex wait_mutex;
	Thread thread;
	static void _thread_start(void *);
	bool quit;
	Command cmd;

	String password;
	int port;
	bool active;

public:
	void start();
	void stop();

	bool is_active() const;

	EditorFileServer();
	~EditorFileServer();
};

#endif // EDITOR_FILE_SERVER_H
