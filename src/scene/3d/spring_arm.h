/**************************************************************************/
/*  spring_arm.h                                                          */
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

#ifndef SPRING_ARM_H
#define SPRING_ARM_H

#include "scene/3d/spatial.h"

class SpringArm : public Spatial {
	GDCLASS(SpringArm, Spatial);

	Ref<Shape> shape;
	Set<RID> excluded_objects;
	float spring_length;
	float current_spring_length;
	bool keep_child_basis;
	uint32_t mask;
	float margin;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_length(float p_length);
	float get_length() const;
	void set_shape(Ref<Shape> p_shape);
	Ref<Shape> get_shape() const;
	void set_mask(uint32_t p_mask);
	uint32_t get_mask();
	void add_excluded_object(RID p_rid);
	bool remove_excluded_object(RID p_rid);
	void clear_excluded_objects();
	float get_hit_length();
	void set_margin(float p_margin);
	float get_margin();

	SpringArm();

private:
	void process_spring();
};

#endif // SPRING_ARM_H
