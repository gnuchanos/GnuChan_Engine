/**************************************************************************/
/*  editor_network_profiler.h                                             */
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

#ifndef EDITOR_NETWORK_PROFILER_H
#define EDITOR_NETWORK_PROFILER_H

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tree.h"

class EditorNetworkProfiler : public VBoxContainer {
	GDCLASS(EditorNetworkProfiler, VBoxContainer)

private:
	Button *activate;
	Button *clear_button;
	Tree *counters_display;
	LineEdit *incoming_bandwidth_text;
	LineEdit *outgoing_bandwidth_text;

	Timer *frame_delay;

	Map<ObjectID, MultiplayerAPI::ProfilingInfo> nodes_data;

	void _update_frame();

	void _activate_pressed();
	void _clear_pressed();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void add_node_frame_data(const MultiplayerAPI::ProfilingInfo p_frame);
	void set_bandwidth(int p_incoming, int p_outgoing);
	bool is_profiling();

	EditorNetworkProfiler();
};

#endif // EDITOR_NETWORK_PROFILER_H
