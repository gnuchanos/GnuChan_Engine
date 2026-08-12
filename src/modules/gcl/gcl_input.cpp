/**************************************************************************/
/*  gcl_input.cpp                                                         */
/**************************************************************************/
/*  GCL input helpers: key-name mapping, IsDown/IsUp/IsKeyDown polling.   */
/**************************************************************************/

#include "core/array.h"
#include "core/input_map.h"
#include "core/os/input.h"
#include "core/os/input_event.h"
#include "core/os/keyboard.h"
#include "core/project_settings.h"
#include "core/reference.h"
#include "gcl_core.h"

#ifdef WINDOWS_ENABLED
#include <windows.h>
#endif

int gcl_keycode_from_name(const String &p_name) {
	if (p_name.length() == 1) {
		CharType ch = p_name[0];
		if (ch >= 'A' && ch <= 'Z') {
			return ch;
		}
		if (ch >= 'a' && ch <= 'z') {
			return ch - 32;
		}
		if (ch >= '0' && ch <= '9') {
			return ch;
		}
		if (ch == ' ') {
			return KEY_SPACE;
		}
	}
	String n = p_name.to_lower();
	if (n == "space") {
		return KEY_SPACE;
	}
	if (n == "enter") {
		return KEY_ENTER;
	}
	if (n == "kp_enter") {
		return KEY_KP_ENTER;
	}
	if (n == "tab") {
		return KEY_TAB;
	}
	if (n == "backspace") {
		return KEY_BACKSPACE;
	}
	if (n == "escape" || n == "esc") {
		return KEY_ESCAPE;
	}
	if (n == "shift") {
		return KEY_SHIFT;
	}
	if (n == "control" || n == "ctrl") {
		return KEY_CONTROL;
	}
	if (n == "alt") {
		return KEY_ALT;
	}
	if (n == "capslock") {
		return KEY_CAPSLOCK;
	}
	if (n == "left") {
		return KEY_LEFT;
	}
	if (n == "right") {
		return KEY_RIGHT;
	}
	if (n == "up") {
		return KEY_UP;
	}
	if (n == "down") {
		return KEY_DOWN;
	}
	if (n == "home") {
		return KEY_HOME;
	}
	if (n == "end") {
		return KEY_END;
	}
	if (n == "pageup") {
		return KEY_PAGEUP;
	}
	if (n == "pagedown") {
		return KEY_PAGEDOWN;
	}
	if (n == "insert") {
		return KEY_INSERT;
	}
	if (n == "delete") {
		return KEY_DELETE;
	}
	if (n.length() >= 2 && n[0] == 'f' && n.substr(1).is_numeric()) {
		int idx = n.substr(1).to_int();
		if (idx >= 1 && idx <= 12) {
			return KEY_F1 + (idx - 1);
		}
	}
	return KEY_UNKNOWN;
}

/* Returns true until the next time the key is used, for one poll:
 * tracks edge. Stored per-instance-set to keep it stateless per call. */
static Map<String, bool> &gcl_up_edges() {
	static Map<String, bool> edges;
	return edges;
}

bool gcl_is_down(const String &p_arg) {
	bool pressed = false;

	if (!InputMap::get_singleton()->has_action(p_arg)) {
		String setting_path = "input/" + p_arg;
		if (ProjectSettings::get_singleton()->has_setting(setting_path)) {
			Dictionary action = ProjectSettings::get_singleton()->get(setting_path);
			InputMap::get_singleton()->add_action(p_arg, action.has("deadzone") ? (float)action["deadzone"] : 0.5f);
			Array events = action["events"];
			for (int ei = 0; ei < events.size(); ei++) {
				Ref<InputEvent> ev = events[ei];
				if (ev.is_valid()) {
					InputMap::get_singleton()->action_add_event(p_arg, ev);
				}
			}
		}
	}

	if (InputMap::get_singleton()->has_action(p_arg)) {
		const List<Ref<InputEvent>> *events = InputMap::get_singleton()->get_action_list(p_arg);
		if (events) {
			for (const List<Ref<InputEvent>>::Element *E = events->front(); E; E = E->next()) {
				Ref<InputEventKey> ev = E->get();
				int sc = ev.is_valid() ? (int)ev->get_scancode() : 0;
				int psc = ev.is_valid() ? (int)ev->get_physical_scancode() : 0;
				bool pd = (sc != 0 && Input::get_singleton()->is_key_pressed(sc)) ||
						(psc != 0 && Input::get_singleton()->is_physical_key_pressed(psc));
#ifdef WINDOWS_ENABLED
				if (!pd) {
					if (sc >= 0x20 && sc <= 0x5A) {
						pd = (GetAsyncKeyState(sc) & 0x8000) != 0;
					}
					if (!pd && psc >= 0x20 && psc <= 0x5A) {
						pd = (GetAsyncKeyState(psc) & 0x8000) != 0;
					}
				}
#endif
				if (pd) {
					pressed = true;
				}
			}
		}
		if (!pressed) {
			pressed = Input::get_singleton()->is_action_pressed(p_arg);
		}
	}

	if (!pressed && !InputMap::get_singleton()->has_action(p_arg)) {
		int keycode = gcl_keycode_from_name(p_arg);
		if (keycode != 0 && keycode != KEY_UNKNOWN) {
			bool key_down = Input::get_singleton()->is_key_pressed(keycode) ||
					Input::get_singleton()->is_physical_key_pressed(keycode);
#ifdef WINDOWS_ENABLED
			if (!key_down && keycode >= 0x20 && keycode <= 0x5A) {
				key_down = (GetAsyncKeyState(keycode) & 0x8000) != 0;
			}
#endif
			pressed = key_down;
		}
	}

	return pressed;
}

bool gcl_is_up(const String &p_arg) {
	// Edge detection: true only on the transition pressed -> released.
	bool now = gcl_is_down(p_arg);
	bool was = gcl_up_edges()[p_arg];
	gcl_up_edges()[p_arg] = now;
	return (was && !now);
}
