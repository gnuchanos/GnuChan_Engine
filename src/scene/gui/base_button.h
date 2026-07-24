/**************************************************************************/
/*  base_button.h                                                         */
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

#ifndef BASE_BUTTON_H
#define BASE_BUTTON_H

#include "scene/gui/control.h"

class ButtonGroup;

class BaseButton : public Control {
	GDCLASS(BaseButton, Control);

public:
	enum ActionMode {
		ACTION_MODE_BUTTON_PRESS,
		ACTION_MODE_BUTTON_RELEASE,
	};

private:
	int button_mask;
	bool toggle_mode;
	bool shortcut_in_tooltip;
	bool keep_pressed_outside;
	bool was_mouse_pressed;
	FocusMode enabled_focus_mode;
	Ref<ShortCut> shortcut;

	ActionMode action_mode;
	struct Status {
		bool pressed;
		bool hovering;
		bool press_attempt;
		bool pressing_inside;

		bool disabled;

	} status;

	Ref<ButtonGroup> button_group;

	void _unpress_group();
	void _pressed();
	void _toggled(bool p_pressed);

	void on_action_event(Ref<InputEvent> p_event);

protected:
	virtual void pressed();
	virtual void toggled(bool p_pressed);
	static void _bind_methods();
	virtual void _gui_input(Ref<InputEvent> p_event);
	virtual void _unhandled_input(Ref<InputEvent> p_event);
	void _notification(int p_what);

	bool _was_pressed_by_mouse() const;

public:
	enum DrawMode {
		DRAW_NORMAL,
		DRAW_PRESSED,
		DRAW_HOVER,
		DRAW_DISABLED,
		DRAW_HOVER_PRESSED,
	};

	DrawMode get_draw_mode() const;

	/* Signals */

	bool is_pressed() const; ///< return whether button is pressed (toggled in)
	bool is_pressing() const; ///< return whether button is pressed (toggled in)
	bool is_hovered() const;

	void set_pressed(bool p_pressed); // Only works in toggle mode.
	void set_pressed_no_signal(bool p_pressed);
	void set_toggle_mode(bool p_on);
	bool is_toggle_mode() const;

	void set_shortcut_in_tooltip(bool p_on);
	bool is_shortcut_in_tooltip_enabled() const;

	void set_disabled(bool p_disabled);
	bool is_disabled() const;

	void set_action_mode(ActionMode p_mode);
	ActionMode get_action_mode() const;

	void set_keep_pressed_outside(bool p_on);
	bool is_keep_pressed_outside() const;

	void set_button_mask(int p_mask);
	int get_button_mask() const;

	void set_enabled_focus_mode(FocusMode p_mode);
	FocusMode get_enabled_focus_mode() const;

	void set_shortcut(const Ref<ShortCut> &p_shortcut);
	Ref<ShortCut> get_shortcut() const;

	virtual String get_tooltip(const Point2 &p_pos) const;

	void set_button_group(const Ref<ButtonGroup> &p_group);
	Ref<ButtonGroup> get_button_group() const;

	BaseButton();
	~BaseButton();
};

VARIANT_ENUM_CAST(BaseButton::DrawMode)
VARIANT_ENUM_CAST(BaseButton::ActionMode)

class ButtonGroup : public Resource {
	GDCLASS(ButtonGroup, Resource);
	friend class BaseButton;
	Set<BaseButton *> buttons;

protected:
	static void _bind_methods();

public:
	BaseButton *get_pressed_button();
	void get_buttons(List<BaseButton *> *r_buttons);
	Array _get_buttons();
	ButtonGroup();
};

#endif // BASE_BUTTON_H
