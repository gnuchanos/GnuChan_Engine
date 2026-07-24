/**************************************************************************/
/*  timer.h                                                               */
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

#ifndef TIMER_H
#define TIMER_H

#include "scene/main/node.h"

class Timer : public Node {
	GDCLASS(Timer, Node);

	float wait_time;
	bool one_shot;
	bool autostart;
	bool processing;
	bool paused;

	double time_left;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	enum TimerProcessMode {
		TIMER_PROCESS_PHYSICS,
		TIMER_PROCESS_IDLE,
	};

	void set_wait_time(float p_time);
	float get_wait_time() const;

	void set_one_shot(bool p_one_shot);
	bool is_one_shot() const;

	void set_autostart(bool p_start);
	bool has_autostart() const;

	void start(float p_time = -1);
	void stop();

	void set_paused(bool p_paused);
	bool is_paused() const;

	bool is_stopped() const;

	float get_time_left() const;

	String get_configuration_warning() const;

	void set_timer_process_mode(TimerProcessMode p_mode);
	TimerProcessMode get_timer_process_mode() const;
	Timer();

private:
	TimerProcessMode timer_process_mode;
	void _set_process(bool p_process, bool p_force = false);
};

VARIANT_ENUM_CAST(Timer::TimerProcessMode);

#endif // TIMER_H
