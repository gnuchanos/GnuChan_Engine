/**************************************************************************/
/*  gltf_camera.h                                                         */
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

#ifndef GLTF_CAMERA_H
#define GLTF_CAMERA_H

#include "core/resource.h"

class Camera;

// Reference and test file:
// https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_015_SimpleCameras.md

class GLTFCamera : public Resource {
	GDCLASS(GLTFCamera, Resource);

private:
	// GLTF has no default camera values, they should always be specified in
	// the GLTF file. Here we default to GnuChan's default camera settings.
	bool perspective = true;
	real_t fov = Math::deg2rad(75.0);
	real_t size_mag = 0.5;
	real_t zfar = 4000.0;
	real_t znear = 0.05;

protected:
	static void _bind_methods();

public:
	bool get_perspective() const { return perspective; }
	void set_perspective(bool p_val) { perspective = p_val; }
	real_t get_fov_size() const { return fov; }
	void set_fov_size(real_t p_val) { fov = p_val; }
	real_t get_size_mag() const { return size_mag; }
	void set_size_mag(real_t p_val) { size_mag = p_val; }
	real_t get_zfar() const { return zfar; }
	void set_zfar(real_t p_val) { zfar = p_val; }
	real_t get_znear() const { return znear; }
	void set_znear(real_t p_val) { znear = p_val; }

	static Ref<GLTFCamera> from_node(const Camera *p_camera);
	Camera *to_node() const;

	static Ref<GLTFCamera> from_dictionary(const Dictionary p_dictionary);
	Dictionary to_dictionary() const;
};

#endif // GLTF_CAMERA_H
