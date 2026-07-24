/**************************************************************************/
/*  gltf_light.h                                                          */
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

#ifndef GLTF_LIGHT_H
#define GLTF_LIGHT_H

#include "../gltf_defines.h"
#include "core/resource.h"

class Light;

// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual

class GLTFLight : public Resource {
	GDCLASS(GLTFLight, Resource)
	friend class GLTFDocument;

protected:
	static void _bind_methods();

private:
	Color color = Color(1.0f, 1.0f, 1.0f);
	float intensity = 1.0f;
	String type;
	float range = INFINITY;
	float inner_cone_angle = 0.0f;
	float outer_cone_angle = Math_TAU / 8.0f;

public:
	Color get_color();
	void set_color(Color p_color);

	float get_intensity();
	void set_intensity(float p_intensity);

	String get_type();
	void set_type(String p_type);

	float get_range();
	void set_range(float p_range);

	float get_inner_cone_angle();
	void set_inner_cone_angle(float p_inner_cone_angle);

	float get_outer_cone_angle();
	void set_outer_cone_angle(float p_outer_cone_angle);

	static Ref<GLTFLight> from_node(const Light *p_light);
	Light *to_node() const;

	static Ref<GLTFLight> from_dictionary(const Dictionary p_dictionary);
	Dictionary to_dictionary() const;
};

#endif // GLTF_LIGHT_H
