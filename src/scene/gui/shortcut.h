/**************************************************************************/
/*  shortcut.h                                                            */
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

#ifndef SHORTCUT_H
#define SHORTCUT_H

#include "core/os/input_event.h"
#include "core/resource.h"

class ShortCut : public Resource {
	GDCLASS(ShortCut, Resource);

	Ref<InputEvent> shortcut;

protected:
	static void _bind_methods();

public:
	void set_shortcut(const Ref<InputEvent> &p_shortcut);
	Ref<InputEvent> get_shortcut() const;
	bool is_shortcut(const Ref<InputEvent> &p_event) const;
	bool is_valid() const;

	String get_as_text() const;

	ShortCut();
};

#endif // SHORTCUT_H
