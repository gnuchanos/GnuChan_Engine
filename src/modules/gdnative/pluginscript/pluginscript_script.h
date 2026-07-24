/**************************************************************************/
/*  pluginscript_script.h                                                 */
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

#ifndef PLUGINSCRIPT_SCRIPT_H
#define PLUGINSCRIPT_SCRIPT_H

// GnuChan imports
#include "core/script_language.h"
// PluginScript imports
#include "pluginscript_language.h"
#include <pluginscript/godot_pluginscript.h>

class PluginScript : public Script {
	GDCLASS(PluginScript, Script);

	friend class PluginScriptInstance;
	friend class PluginScriptLanguage;

private:
	godot_pluginscript_script_data *_data;
	const godot_pluginscript_script_desc *_desc;
	PluginScriptLanguage *_language;
	bool _tool;
	bool _valid;

	Ref<Script> _ref_base_parent;
	StringName _native_parent;
	SelfList<PluginScript> _script_list;

	Map<StringName, int> _member_lines;
	Map<StringName, Variant> _properties_default_values;
	Map<StringName, PropertyInfo> _properties_info;
	Map<StringName, MethodInfo> _signals_info;
	Map<StringName, MethodInfo> _methods_info;
	Map<StringName, MultiplayerAPI::RPCMode> _variables_rset_mode;
	Map<StringName, MultiplayerAPI::RPCMode> _methods_rpc_mode;

	Set<Object *> _instances;
	//exported members
	String _source;
	String _path;
	StringName _name;

protected:
	static void _bind_methods();

	bool inherits_script(const Ref<Script> &p_script) const;

	PluginScriptInstance *_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, Variant::CallError &r_error);
	Variant _new(const Variant **p_args, int p_argcount, Variant::CallError &r_error);

#ifdef TOOLS_ENABLED
	Set<PlaceHolderScriptInstance *> placeholders;
	//void _update_placeholder(PlaceHolderScriptInstance *p_placeholder);
	virtual void _placeholder_erased(PlaceHolderScriptInstance *p_placeholder);
#endif
public:
	virtual bool can_instance() const;

	virtual Ref<Script> get_base_script() const; //for script inheritance

	virtual StringName get_instance_base_type() const; // this may not work in all scripts, will return empty if so
	virtual ScriptInstance *instance_create(Object *p_this);
	virtual bool instance_has(const Object *p_this) const;

	virtual bool has_source_code() const;
	virtual String get_source_code() const;
	virtual void set_source_code(const String &p_code);
	virtual Error reload(bool p_keep_state = false);
	// TODO: load_source_code only allow utf-8 file, should handle bytecode as well ?
	virtual Error load_source_code(const String &p_path);

	virtual bool has_method(const StringName &p_method) const;
	virtual MethodInfo get_method_info(const StringName &p_method) const;

	bool has_property(const StringName &p_method) const;
	PropertyInfo get_property_info(const StringName &p_property) const;

	bool is_tool() const { return _tool; }
	bool is_valid() const { return true; }

	virtual ScriptLanguage *get_language() const;

	virtual bool has_script_signal(const StringName &p_signal) const;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const;

	virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const;

	virtual void update_exports();
	virtual void get_script_method_list(List<MethodInfo> *r_methods) const;
	virtual void get_script_property_list(List<PropertyInfo> *r_properties) const;

	virtual int get_member_line(const StringName &p_member) const;

	MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const;
	MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const;

	PluginScript();
	void init(PluginScriptLanguage *language);
	virtual ~PluginScript();
};

#endif // PLUGINSCRIPT_SCRIPT_H
