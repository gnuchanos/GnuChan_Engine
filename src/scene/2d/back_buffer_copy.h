/**************************************************************************/
/*  back_buffer_copy.h                                                    */
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

#ifndef BACK_BUFFER_COPY_H
#define BACK_BUFFER_COPY_H

#include "scene/2d/node_2d.h"

class BackBufferCopy : public Node2D {
	GDCLASS(BackBufferCopy, Node2D);

public:
	enum CopyMode {
		COPY_MODE_DISABLED,
		COPY_MODE_RECT,
		COPY_MODE_VIEWPORT
	};

private:
	Rect2 rect;
	CopyMode copy_mode;

	void _update_copy_mode();

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

public:
#ifdef TOOLS_ENABLED
	Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
#endif

	void set_rect(const Rect2 &p_rect);
	Rect2 get_rect() const;
	Rect2 get_anchorable_rect() const;

	void set_copy_mode(CopyMode p_mode);
	CopyMode get_copy_mode() const;

	BackBufferCopy();
	~BackBufferCopy();
};

VARIANT_ENUM_CAST(BackBufferCopy::CopyMode);

#endif // BACK_BUFFER_COPY_H
