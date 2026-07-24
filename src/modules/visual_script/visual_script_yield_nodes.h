/**************************************************************************/
/*  visual_script_yield_nodes.h                                           */
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

#ifndef VISUAL_SCRIPT_YIELD_NODES_H
#define VISUAL_SCRIPT_YIELD_NODES_H

#include "visual_script.h"

class VisualScriptYield : public VisualScriptNode {
	GDCLASS(VisualScriptYield, VisualScriptNode);

public:
	enum YieldMode {
		YIELD_RETURN,
		YIELD_FRAME,
		YIELD_PHYSICS_FRAME,
		YIELD_WAIT

	};

private:
	YieldMode yield_mode;
	float wait_time;

protected:
	virtual void _validate_property(PropertyInfo &property) const;

	static void _bind_methods();

public:
	virtual int get_output_sequence_port_count() const;
	virtual bool has_input_sequence_port() const;

	virtual String get_output_sequence_port_text(int p_port) const;

	virtual int get_input_value_port_count() const;
	virtual int get_output_value_port_count() const;

	virtual PropertyInfo get_input_value_port_info(int p_idx) const;
	virtual PropertyInfo get_output_value_port_info(int p_idx) const;

	virtual String get_caption() const;
	virtual String get_text() const;
	virtual String get_category() const { return "functions"; }

	void set_yield_mode(YieldMode p_mode);
	YieldMode get_yield_mode();

	void set_wait_time(float p_time);
	float get_wait_time();

	virtual VisualScriptNodeInstance *instance(VisualScriptInstance *p_instance);

	VisualScriptYield();
};
VARIANT_ENUM_CAST(VisualScriptYield::YieldMode)

class VisualScriptYieldSignal : public VisualScriptNode {
	GDCLASS(VisualScriptYieldSignal, VisualScriptNode);

public:
	enum CallMode {
		CALL_MODE_SELF,
		CALL_MODE_NODE_PATH,
		CALL_MODE_INSTANCE,

	};

private:
	CallMode call_mode;
	StringName base_type;
	NodePath base_path;
	StringName signal;

	Node *_get_base_node() const;
	StringName _get_base_type() const;

protected:
	virtual void _validate_property(PropertyInfo &property) const;

	static void _bind_methods();

public:
	virtual int get_output_sequence_port_count() const;
	virtual bool has_input_sequence_port() const;

	virtual String get_output_sequence_port_text(int p_port) const;

	virtual int get_input_value_port_count() const;
	virtual int get_output_value_port_count() const;

	virtual PropertyInfo get_input_value_port_info(int p_idx) const;
	virtual PropertyInfo get_output_value_port_info(int p_idx) const;

	virtual String get_caption() const;
	virtual String get_text() const;
	virtual String get_category() const { return "functions"; }

	void set_base_type(const StringName &p_type);
	StringName get_base_type() const;

	void set_signal(const StringName &p_type);
	StringName get_signal() const;

	void set_base_path(const NodePath &p_type);
	NodePath get_base_path() const;

	void set_call_mode(CallMode p_mode);
	CallMode get_call_mode() const;

	virtual VisualScriptNodeInstance *instance(VisualScriptInstance *p_instance);

	VisualScriptYieldSignal();
};

VARIANT_ENUM_CAST(VisualScriptYieldSignal::CallMode);

void register_visual_script_yield_nodes();

#endif // VISUAL_SCRIPT_YIELD_NODES_H
