/**************************************************************************/
/*  camera_win.cpp                                                        */
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

#include "camera_win.h"

///@TODO sorry guys, I got about 80% through implementing this using DirectShow only
// to find out Microsoft deprecated half the API and its replacement is as confusing
// as they could make it. Joey suggested looking into libuvc which offers a more direct
// route to webcams over USB and this is very promising but it wouldn't compile on
// windows for me...I've gutted the classes I implemented DirectShow in just to have
// a skeleton for someone to work on, mail me for more details or if you want a copy....

//////////////////////////////////////////////////////////////////////////
// CameraFeedWindows - Subclass for our camera feed on windows

/// @TODO need to implement this

class CameraFeedWindows : public CameraFeed {
private:
protected:
public:
	CameraFeedWindows();
	virtual ~CameraFeedWindows();

	bool activate_feed();
	void deactivate_feed();
};

CameraFeedWindows::CameraFeedWindows(){
	///@TODO implement this, should store information about our available camera
};

CameraFeedWindows::~CameraFeedWindows() {
	// make sure we stop recording if we are!
	if (is_active()) {
		deactivate_feed();
	};

	///@TODO free up anything used by this
};

bool CameraFeedWindows::activate_feed() {
	///@TODO this should activate our camera and start the process of capturing frames

	return true;
};

///@TODO we should probably have a callback method here that is being called by the
// camera API which provides frames and call back into the CameraServer to update our texture

void CameraFeedWindows::deactivate_feed(){
	///@TODO this should deactivate our camera and stop the process of capturing frames
};

//////////////////////////////////////////////////////////////////////////
// CameraWindows - Subclass for our camera server on windows

void CameraWindows::add_active_cameras(){
	///@TODO scan through any active cameras and create CameraFeedWindows objects for them
};

CameraWindows::CameraWindows() {
	// Find cameras active right now
	add_active_cameras();

	// need to add something that will react to devices being connected/removed...
};

CameraWindows::~CameraWindows(){

};
