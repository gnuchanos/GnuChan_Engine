/**************************************************************************/
/*  shader_compiler_gles4.h                                               */
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

#ifndef SHADER_COMPILER_GLES4_H
#define SHADER_COMPILER_GLES4_H

#include "core/pair.h"
#include "servers/visual/shader_language.h"
#include "servers/visual/shader_types.h"
#include "servers/visual_server.h"

class ShaderCompilerGLES4 {
public:
	struct IdentifierActions {
		Map<StringName, Pair<int *, int>> render_mode_values;
		Map<StringName, bool *> render_mode_flags;
		Map<StringName, bool *> usage_flag_pointers;
		Map<StringName, bool *> write_flag_pointers;

		Map<StringName, ShaderLanguage::ShaderNode::Uniform> *uniforms;
	};

	struct GeneratedCode {
		Vector<CharString> defines;
		Vector<StringName> texture_uniforms;
		Vector<ShaderLanguage::DataType> texture_types;
		Vector<ShaderLanguage::ShaderNode::Uniform::Hint> texture_hints;

		Vector<uint32_t> uniform_offsets;
		uint32_t uniform_total_size;
		String uniforms;
		String vertex_global;
		String vertex;
		String fragment_global;
		String fragment;
		String light;

		bool uses_fragment_time;
		bool uses_vertex_time;
	};

private:
	ShaderLanguage parser;

	struct DefaultIdentifierActions {
		Map<StringName, String> renames;
		Map<StringName, String> render_mode_defines;
		Map<StringName, String> usage_defines;
	};

	void _dump_function_deps(const ShaderLanguage::ShaderNode *p_node, const StringName &p_for_func, const Map<StringName, String> &p_func_code, String &r_to_add, Set<StringName> &added);
	String _dump_node_code(const ShaderLanguage::Node *p_node, int p_level, GeneratedCode &r_gen_code, IdentifierActions &p_actions, const DefaultIdentifierActions &p_default_actions, bool p_assigning, bool p_use_scope = true);

	const ShaderLanguage::ShaderNode *shader;
	const ShaderLanguage::FunctionNode *function;
	StringName current_func_name;
	StringName vertex_name;
	StringName fragment_name;
	StringName light_name;
	StringName time_name;

	Set<StringName> used_name_defines;
	Set<StringName> used_flag_pointers;
	Set<StringName> used_rmode_defines;
	Set<StringName> internal_functions;
	Set<StringName> fragment_varyings;

	DefaultIdentifierActions actions[VS::SHADER_MAX];

public:
	Error compile(VS::ShaderMode p_mode, const String &p_code, IdentifierActions *p_actions, const String &p_path, GeneratedCode &r_gen_code);

	ShaderCompilerGLES4();
};

#endif // SHADER_COMPILER_GLES4_H