/**************************************************************************/
/*  pluginscript_instance.h                                               */
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

#ifndef PLUGINSCRIPT_INSTANCE_H
#define PLUGINSCRIPT_INSTANCE_H

// GnuChan imports
#include "core/script_language.h"

// PluginScript imports
#include <pluginscript/godot_pluginscript.h>

class PluginScript;

class PluginScriptInstance : public ScriptInstance {
	friend class PluginScript;

private:
	Ref<PluginScript> _script;
	Object *_owner;
	Variant _owner_variant;
	godot_pluginscript_instance_data *_data;
	const godot_pluginscript_instance_desc *_desc;

public:
	_FORCE_INLINE_ Object *get_owner() { return _owner; }

	virtual bool set(const StringName &p_name, const Variant &p_value);
	virtual bool get(const StringName &p_name, Variant &r_ret) const;
	virtual void get_property_list(List<PropertyInfo> *p_properties) const;
	virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = nullptr) const;

	virtual void get_method_list(List<MethodInfo> *p_list) const;
	virtual bool has_method(const StringName &p_method) const;

	virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);

	// Rely on default implementations provided by ScriptInstance for the moment.
	// Note that multilevel call could be removed in 3.0 release, so stay tuned
	// (see https://github.com/gnuchanos/GnuChan_Engine/qa/9244/can-override-the-_ready-and-_process-functions-child-classes)
	//virtual void call_multilevel(const StringName& p_method,const Variant** p_args,int p_argcount);
	//virtual void call_multilevel_reversed(const StringName& p_method,const Variant** p_args,int p_argcount);

	virtual void notification(int p_notification);

	virtual Ref<Script> get_script() const;

	virtual ScriptLanguage *get_language();

	virtual MultiplayerAPI::RPCMode get_rpc_mode(const StringName &p_method) const;
	virtual MultiplayerAPI::RPCMode get_rset_mode(const StringName &p_variable) const;

	virtual void refcount_incremented();
	virtual bool refcount_decremented();

	PluginScriptInstance();
	bool init(PluginScript *p_script, Object *p_owner);
	virtual ~PluginScriptInstance();
};

#endif // PLUGINSCRIPT_INSTANCE_H
