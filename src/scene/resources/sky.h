/**************************************************************************/
/*  sky.h                                                                 */
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

#ifndef SKY_H
#define SKY_H

#include "core/os/thread.h"
#include "scene/resources/texture.h"

class Sky : public Resource {
	GDCLASS(Sky, Resource);

public:
	enum RadianceSize {
		RADIANCE_SIZE_32,
		RADIANCE_SIZE_64,
		RADIANCE_SIZE_128,
		RADIANCE_SIZE_256,
		RADIANCE_SIZE_512,
		RADIANCE_SIZE_1024,
		RADIANCE_SIZE_2048,
		RADIANCE_SIZE_MAX
	};

private:
	RadianceSize radiance_size;

protected:
	static void _bind_methods();
	virtual void _radiance_changed() = 0;

public:
	void set_radiance_size(RadianceSize p_size);
	RadianceSize get_radiance_size() const;
	Sky();
};

VARIANT_ENUM_CAST(Sky::RadianceSize)

class PanoramaSky : public Sky {
	GDCLASS(PanoramaSky, Sky);

private:
	RID sky;
	Ref<Texture> panorama;

protected:
	static void _bind_methods();
	virtual void _radiance_changed();

public:
	void set_panorama(const Ref<Texture> &p_panorama);
	Ref<Texture> get_panorama() const;

	virtual RID get_rid() const;

	PanoramaSky();
	~PanoramaSky();
};

class ProceduralSky : public Sky {
	GDCLASS(ProceduralSky, Sky);

public:
	enum TextureSize {
		TEXTURE_SIZE_256,
		TEXTURE_SIZE_512,
		TEXTURE_SIZE_1024,
		TEXTURE_SIZE_2048,
		TEXTURE_SIZE_4096,
		TEXTURE_SIZE_MAX
	};

private:
	Thread sky_thread;
	Color sky_top_color;
	Color sky_horizon_color;
	float sky_curve;
	float sky_energy;

	Color ground_bottom_color;
	Color ground_horizon_color;
	float ground_curve;
	float ground_energy;

	Color sun_color;
	float sun_latitude;
	float sun_longitude;
	float sun_angle_min;
	float sun_angle_max;
	float sun_curve;
	float sun_energy;

	TextureSize texture_size;

	RID sky;
	RID texture;
	Ref<Image> panorama;

	bool update_queued;
	bool regen_queued;

	bool first_time;

	void _thread_done(const Ref<Image> &p_image);
	static void _thread_function(void *p_ud);

protected:
	static void _bind_methods();
	virtual void _radiance_changed();

	Ref<Image> _generate_sky();
	void _update_sky();

	void _queue_update();

public:
	void set_sky_top_color(const Color &p_sky_top);
	Color get_sky_top_color() const;

	void set_sky_horizon_color(const Color &p_sky_horizon);
	Color get_sky_horizon_color() const;

	void set_sky_curve(float p_curve);
	float get_sky_curve() const;

	void set_sky_energy(float p_energy);
	float get_sky_energy() const;

	void set_ground_bottom_color(const Color &p_ground_bottom);
	Color get_ground_bottom_color() const;

	void set_ground_horizon_color(const Color &p_ground_horizon);
	Color get_ground_horizon_color() const;

	void set_ground_curve(float p_curve);
	float get_ground_curve() const;

	void set_ground_energy(float p_energy);
	float get_ground_energy() const;

	void set_sun_color(const Color &p_sun);
	Color get_sun_color() const;

	void set_sun_latitude(float p_angle);
	float get_sun_latitude() const;

	void set_sun_longitude(float p_angle);
	float get_sun_longitude() const;

	void set_sun_angle_min(float p_angle);
	float get_sun_angle_min() const;

	void set_sun_angle_max(float p_angle);
	float get_sun_angle_max() const;

	void set_sun_curve(float p_curve);
	float get_sun_curve() const;

	void set_sun_energy(float p_energy);
	float get_sun_energy() const;

	void set_texture_size(TextureSize p_size);
	TextureSize get_texture_size() const;

	Ref<Image> get_data() const;

	virtual RID get_rid() const;

	ProceduralSky(bool p_desaturate = false);
	~ProceduralSky();
};

VARIANT_ENUM_CAST(ProceduralSky::TextureSize)

#endif // SKY_H
