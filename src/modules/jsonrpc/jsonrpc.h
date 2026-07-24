/**************************************************************************/
/*  jsonrpc.h                                                             */
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

#ifndef JSONRPC_H
#define JSONRPC_H

#include "core/object.h"
#include "core/variant.h"

class JSONRPC : public Object {
	GDCLASS(JSONRPC, Object)

	Map<String, Object *> method_scopes;

protected:
	static void _bind_methods();

public:
	JSONRPC();
	~JSONRPC();

	enum ErrorCode {
		PARSE_ERROR = -32700,
		INVALID_REQUEST = -32600,
		METHOD_NOT_FOUND = -32601,
		INVALID_PARAMS = -32602,
		INTERNAL_ERROR = -32603,
	};

	Dictionary make_response_error(int p_code, const String &p_message, const Variant &p_id = Variant()) const;
	Dictionary make_response(const Variant &p_value, const Variant &p_id);
	Dictionary make_notification(const String &p_method, const Variant &p_params);
	Dictionary make_request(const String &p_method, const Variant &p_params, const Variant &p_id);

	Variant process_action(const Variant &p_action, bool p_process_arr_elements = false);
	String process_string(const String &p_input);

	void set_scope(const String &p_scope, Object *p_obj);
};

VARIANT_ENUM_CAST(JSONRPC::ErrorCode);

#endif // JSONRPC_H
