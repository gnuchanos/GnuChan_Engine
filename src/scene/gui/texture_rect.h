/**************************************************************************/
/*  texture_rect.h                                                        */
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

#ifndef TEXTURE_RECT_H
#define TEXTURE_RECT_H

#include "scene/gui/control.h"

class TextureRect : public Control {
	GDCLASS(TextureRect, Control);

public:
	enum StretchMode {
		STRETCH_SCALE_ON_EXPAND, //default, for backwards compatibility
		STRETCH_SCALE,
		STRETCH_TILE,
		STRETCH_KEEP,
		STRETCH_KEEP_CENTERED,
		STRETCH_KEEP_ASPECT,
		STRETCH_KEEP_ASPECT_CENTERED,
		STRETCH_KEEP_ASPECT_COVERED,
	};

private:
	bool expand;
	bool hflip;
	bool vflip;
	Ref<Texture> texture;
	StretchMode stretch_mode;

	void _texture_changed();

protected:
	void _notification(int p_what);
	virtual Size2 get_minimum_size() const;
	static void _bind_methods();

public:
	void set_texture(const Ref<Texture> &p_tex);
	Ref<Texture> get_texture() const;

	void set_expand(bool p_expand);
	bool has_expand() const;

	void set_stretch_mode(StretchMode p_mode);
	StretchMode get_stretch_mode() const;

	void set_flip_h(bool p_flip);
	bool is_flipped_h() const;

	void set_flip_v(bool p_flip);
	bool is_flipped_v() const;

	TextureRect();
	~TextureRect();
};

VARIANT_ENUM_CAST(TextureRect::StretchMode);

#endif // TEXTURE_RECT_H
