/**************************************************************************/
/*  camera_server.h                                                       */
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

#ifndef CAMERA_SERVER_H
#define CAMERA_SERVER_H

#include "core/object.h"
#include "core/os/thread_safe.h"
#include "core/reference.h"
#include "core/rid.h"
#include "core/variant.h"

/**
	@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>

	The camera server is a singleton object that gives access to the various
	camera feeds that can be used as the background for our environment.
**/

class CameraFeed;

class CameraServer : public Object {
	GDCLASS(CameraServer, Object);
	_THREAD_SAFE_CLASS_

public:
	enum FeedImage {
		FEED_RGBA_IMAGE = 0,
		FEED_YCBCR_IMAGE = 0,
		FEED_Y_IMAGE = 0,
		FEED_CBCR_IMAGE = 1,
		FEED_IMAGES = 2
	};

	typedef CameraServer *(*CreateFunc)();

private:
protected:
	static CreateFunc create_func;

	Vector<Ref<CameraFeed>> feeds;

	static CameraServer *singleton;

	static void _bind_methods();

	template <class T>
	static CameraServer *_create_builtin() {
		return memnew(T);
	}

public:
	static CameraServer *get_singleton();

	template <class T>
	static void make_default() {
		create_func = _create_builtin<T>;
	}

	static CameraServer *create() {
		CameraServer *server = create_func ? create_func() : memnew(CameraServer);
		return server;
	};

	// Right now we identify our feed by it's ID when it's used in the background.
	// May see if we can change this to purely relying on CameraFeed objects or by name.
	int get_free_id();
	int get_feed_index(int p_id);
	Ref<CameraFeed> get_feed_by_id(int p_id);

	// add and remove feeds
	void add_feed(const Ref<CameraFeed> &p_feed);
	void remove_feed(const Ref<CameraFeed> &p_feed);

	// get our feeds
	Ref<CameraFeed> get_feed(int p_index);
	int get_feed_count();
	Array get_feeds();

	RID feed_texture(int p_id, FeedImage p_texture);

	CameraServer();
	~CameraServer();
};

VARIANT_ENUM_CAST(CameraServer::FeedImage);

#endif // CAMERA_SERVER_H
