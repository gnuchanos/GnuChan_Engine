/**************************************************************************/
/*  gltf_animation.h                                                      */
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

#ifndef GLTF_ANIMATION_H
#define GLTF_ANIMATION_H

#include "core/resource.h"

class GLTFAnimation : public Resource {
	GDCLASS(GLTFAnimation, Resource);

protected:
	static void _bind_methods();

public:
	enum Interpolation {
		INTERP_LINEAR,
		INTERP_STEP,
		INTERP_CATMULLROMSPLINE,
		INTERP_CUBIC_SPLINE,
	};

	template <class T>
	struct Channel {
		Interpolation interpolation;
		Vector<float> times;
		Vector<T> values;
	};

	struct Track {
		Channel<Vector3> translation_track;
		Channel<Quat> rotation_track;
		Channel<Vector3> scale_track;
		Vector<Channel<float>> weight_tracks;
	};

public:
	bool get_loop() const;
	void set_loop(bool p_val);
	Map<int, GLTFAnimation::Track> &get_tracks();
	GLTFAnimation();

private:
	bool loop = false;
	Map<int, Track> tracks;
};

#endif // GLTF_ANIMATION_H
