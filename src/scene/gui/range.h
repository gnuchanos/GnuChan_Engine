/**************************************************************************/
/*  range.h                                                               */
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

#ifndef RANGE_H
#define RANGE_H

#include "scene/gui/control.h"

class Range : public Control {
	GDCLASS(Range, Control);

	struct Shared {
		double val, min, max;
		double step, page;
		bool exp_ratio;
		bool allow_greater;
		bool allow_lesser;
		Set<Range *> owners;
		void emit_value_changed();
		void emit_changed(const char *p_what = "");
	};

	Shared *shared;

	void _ref_shared(Shared *p_shared);
	void _unref_shared();

	void _share(Node *p_range);

	void _value_changed_notify();
	void _changed_notify(const char *p_what = "");

protected:
	virtual void _value_changed(double) {}

	static void _bind_methods();

	bool _rounded_values;

public:
	void set_value(double p_val);
	void set_value_no_signal(double p_val);
	void set_min(double p_min);
	void set_max(double p_max);
	void set_step(double p_step);
	void set_page(double p_page);
	void set_as_ratio(double p_value);

	double get_value() const;
	double get_min() const;
	double get_max() const;
	double get_step() const;
	double get_page() const;
	double get_as_ratio() const;

	void set_use_rounded_values(bool p_enable);
	bool is_using_rounded_values() const;

	void set_exp_ratio(bool p_enable);
	bool is_ratio_exp() const;

	void set_allow_greater(bool p_allow);
	bool is_greater_allowed() const;

	void set_allow_lesser(bool p_allow);
	bool is_lesser_allowed() const;

	void share(Range *p_range);
	void unshare();

	virtual String get_configuration_warning() const;

	Range();
	~Range();
};

#endif // RANGE_H
