/**************************************************************************/
/*  button.h                                                              */
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

#ifndef BUTTON_H
#define BUTTON_H

#include "scene/gui/base_button.h"

class Button : public BaseButton {
	GDCLASS(Button, BaseButton);

public:
	enum TextAlign {
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

private:
	bool flat;
	String text;
	String xl_text;
	Ref<Texture> icon;
	bool expand_icon;
	bool clip_text;
	TextAlign align;
	TextAlign icon_align;
	float _internal_margin[4];

	void _texture_changed();

protected:
	void _set_internal_margin(Margin p_margin, float p_value);
	void _notification(int p_what);
	static void _bind_methods();

public:
	virtual Size2 get_minimum_size() const;

	void set_text(const String &p_text);
	String get_text() const;

	void set_icon(const Ref<Texture> &p_icon);
	Ref<Texture> get_icon() const;

	void set_expand_icon(bool p_expand_icon);
	bool is_expand_icon() const;

	void set_flat(bool p_flat);
	bool is_flat() const;

	void set_clip_text(bool p_clip_text);
	bool get_clip_text() const;

	void set_text_align(TextAlign p_align);
	TextAlign get_text_align() const;

	void set_icon_align(TextAlign p_align);
	TextAlign get_icon_align() const;

	Button(const String &p_text = String());
	~Button();
};

VARIANT_ENUM_CAST(Button::TextAlign);

#endif // BUTTON_H
