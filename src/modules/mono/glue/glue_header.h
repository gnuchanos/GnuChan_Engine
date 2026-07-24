/**************************************************************************/
/*  glue_header.h                                                         */
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

#ifndef GLUE_HEADER_H
#define GLUE_HEADER_H

#ifdef MONO_GLUE_ENABLED

#include "base_object_glue.h"
#include "collections_glue.h"
#include "gd_glue.h"
#include "nodepath_glue.h"
#include "rid_glue.h"
#include "string_glue.h"

/**
 * Registers internal calls that were not generated. This function is called
 * from the generated GodotSharpBindings::register_generated_icalls() function.
 */
void godot_register_glue_header_icalls() {
	godot_register_collections_icalls();
	godot_register_gd_icalls();
	godot_register_nodepath_icalls();
	godot_register_object_icalls();
	godot_register_rid_icalls();
	godot_register_string_icalls();
}

// Used by the generated glue

#include "core/array.h"
#include "core/class_db.h"
#include "core/dictionary.h"
#include "core/engine.h"
#include "core/method_bind.h"
#include "core/node_path.h"
#include "core/object.h"
#include "core/reference.h"
#include "core/typedefs.h"
#include "core/ustring.h"

#include "../mono_gd/gd_mono_class.h"
#include "../mono_gd/gd_mono_internals.h"
#include "../mono_gd/gd_mono_utils.h"

#define GODOTSHARP_INSTANCE_OBJECT(m_instance, m_type) \
	static ClassDB::ClassInfo *ci = NULL;              \
	if (!ci) {                                         \
		ci = ClassDB::classes.getptr(m_type);          \
	}                                                  \
	Object *m_instance = ci->creation_func();

#include "arguments_vector.h"

#endif // MONO_GLUE_ENABLED

#endif // GLUE_HEADER_H
