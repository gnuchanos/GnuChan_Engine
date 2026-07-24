/**************************************************************************/
/*  class_db_api_json.h                                                   */
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

#ifndef CLASS_DB_API_JSON_H
#define CLASS_DB_API_JSON_H

// 'core/method_bind.h' defines DEBUG_METHODS_ENABLED, but it looks like we
// cannot include it here. That's why we include it through 'core/class_db.h'.
#include "core/class_db.h"

#ifdef DEBUG_METHODS_ENABLED

#include "core/ustring.h"

void class_db_api_to_json(const String &p_output_file, ClassDB::APIType p_api);

#endif // DEBUG_METHODS_ENABLED

#endif // CLASS_DB_API_JSON_H
