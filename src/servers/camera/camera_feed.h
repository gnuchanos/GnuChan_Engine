/**************************************************************************/
/*  camera_feed.h                                                         */
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

#ifndef CAMERA_FEED_H
#define CAMERA_FEED_H

#include "core/image.h"
#include "core/math/transform_2d.h"
#include "servers/camera_server.h"
#include "servers/visual_server.h"

/**
	@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>

	The camera server is a singleton object that gives access to the various
	camera feeds that can be used as the background for our environment.
**/

class CameraFeed : public Reference {
	GDCLASS(CameraFeed, Reference);

public:
	enum FeedDataType {
		FEED_NOIMAGE, // we don't have an image yet
		FEED_RGB, // our texture will contain a normal RGB texture that can be used directly
		FEED_YCBCR, // our texture will contain a YCbCr texture that needs to be converted to RGB before output
		FEED_YCBCR_SEP // our camera is split into two textures, first plane contains Y data, second plane contains CbCr data
	};

	enum FeedPosition {
		FEED_UNSPECIFIED, // we have no idea
		FEED_FRONT, // this is a camera on the front of the device
		FEED_BACK // this is a camera on the back of the device
	};

private:
	int id; // unique id for this, for internal use in case feeds are removed
	int base_width;
	int base_height;

protected:
	String name; // name of our camera feed
	FeedDataType datatype; // type of texture data stored
	FeedPosition position; // position of camera on the device
	Transform2D transform; // display transform

	bool active; // only when active do we actually update the camera texture each frame
	RID texture[CameraServer::FEED_IMAGES]; // texture images needed for this

	static void _bind_methods();

public:
	int get_id() const;
	bool is_active() const;
	void set_active(bool p_is_active);

	String get_name() const;
	void set_name(String p_name);

	int get_base_width() const;
	int get_base_height() const;

	FeedPosition get_position() const;
	void set_position(FeedPosition p_position);

	Transform2D get_transform() const;
	void set_transform(const Transform2D &p_transform);

	RID get_texture(CameraServer::FeedImage p_which);

	CameraFeed();
	CameraFeed(String p_name, FeedPosition p_position = CameraFeed::FEED_UNSPECIFIED);
	virtual ~CameraFeed();

	FeedDataType get_datatype() const;
	void set_RGB_img(const Ref<Image> &p_rgb_img);
	void set_YCbCr_img(const Ref<Image> &p_ycbcr_img);
	void set_YCbCr_imgs(const Ref<Image> &p_y_img, const Ref<Image> &p_cbcr_img);
	void allocate_texture(int p_width, int p_height, Image::Format p_format, VisualServer::TextureType p_texture_type, FeedDataType p_data_type);

	virtual bool activate_feed();
	virtual void deactivate_feed();
};

VARIANT_ENUM_CAST(CameraFeed::FeedDataType);
VARIANT_ENUM_CAST(CameraFeed::FeedPosition);

#endif // CAMERA_FEED_H
