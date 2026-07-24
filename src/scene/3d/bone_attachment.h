/**************************************************************************/
/*  bone_attachment.h                                                     */
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

#ifndef BONE_ATTACHMENT_H
#define BONE_ATTACHMENT_H

#include "scene/3d/skeleton.h"

class BoneAttachment : public Spatial {
	GDCLASS(BoneAttachment, Spatial);

	bool bound;
	String bone_name;

	void _check_bind();
	void _check_unbind();

protected:
	virtual void _validate_property(PropertyInfo &property) const;
	void _notification(int p_what);

	static void _bind_methods();

public:
	void set_bone_name(const String &p_name);
	String get_bone_name() const;

	BoneAttachment();
};

#endif // BONE_ATTACHMENT_H
