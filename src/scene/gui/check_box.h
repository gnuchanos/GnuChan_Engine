/**************************************************************************/
/*  check_box.h                                                           */
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

#ifndef CHECK_BOX_H
#define CHECK_BOX_H

#include "scene/gui/button.h"
/**
@a Author: GnuChan Engine contributors <gnuchanos@gmail.com>
*/
class CheckBox : public Button {
	GDCLASS(CheckBox, Button);

protected:
	Size2 get_icon_size() const;
	Size2 get_minimum_size() const;
	void _notification(int p_what);

	bool is_radio();

public:
	CheckBox(const String &p_text = String());
	~CheckBox();
};

#endif // CHECK_BOX_H
