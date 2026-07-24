/**************************************************************************/
/*  input_map.h                                                           */
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

#ifndef INPUT_MAP_H
#define INPUT_MAP_H

#include "core/object.h"
#include "core/os/input_event.h"

class InputMap : public Object {
	GDCLASS(InputMap, Object);

public:
	/**
	 * A special value used to signify that a given Action can be triggered by any device
	 */
	static int ALL_DEVICES;

	struct Action {
		int id;
		float deadzone;
		List<Ref<InputEvent>> inputs;
	};

private:
	static InputMap *singleton;

	mutable Map<StringName, Action> input_map;

	List<Ref<InputEvent>>::Element *_find_event(Action &p_action, const Ref<InputEvent> &p_event, bool p_exact_match = false, bool *p_pressed = nullptr, float *p_strength = nullptr, float *p_raw_strength = nullptr) const;

	Array _get_action_list(const StringName &p_action);
	Array _get_actions();

protected:
	static void _bind_methods();

public:
	static _FORCE_INLINE_ InputMap *get_singleton() { return singleton; }

	bool has_action(const StringName &p_action) const;
	List<StringName> get_actions() const;
	void add_action(const StringName &p_action, float p_deadzone = 0.5);
	void erase_action(const StringName &p_action);

	float action_get_deadzone(const StringName &p_action);
	void action_set_deadzone(const StringName &p_action, float p_deadzone);
	void action_add_event(const StringName &p_action, const Ref<InputEvent> &p_event);
	bool action_has_event(const StringName &p_action, const Ref<InputEvent> &p_event);
	void action_erase_event(const StringName &p_action, const Ref<InputEvent> &p_event);
	void action_erase_events(const StringName &p_action);

	const List<Ref<InputEvent>> *get_action_list(const StringName &p_action);
	bool event_is_action(const Ref<InputEvent> &p_event, const StringName &p_action, bool p_exact_match = false) const;
	bool event_get_action_status(const Ref<InputEvent> &p_event, const StringName &p_action, bool p_exact_match = false, bool *p_pressed = nullptr, float *p_strength = nullptr, float *p_raw_strength = nullptr) const;

	const Map<StringName, Action> &get_action_map() const;
	void load_from_globals();
	void load_default();

	String suggest_actions(const StringName &p_action) const;

	InputMap();
};

#endif // INPUT_MAP_H
