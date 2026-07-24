/**************************************************************************/
/*  string_utils.h                                                        */
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

#ifndef MONO_STRING_UTILS_H
#define MONO_STRING_UTILS_H

#include "core/ustring.h"
#include "core/variant.h"

#include <stdarg.h>

String sformat(const String &p_text, const Variant &p1 = Variant(), const Variant &p2 = Variant(), const Variant &p3 = Variant(), const Variant &p4 = Variant(), const Variant &p5 = Variant());

#ifdef TOOLS_ENABLED
bool is_csharp_keyword(const String &p_name);

String escape_csharp_keyword(const String &p_name);
#endif

Error read_all_file_utf8(const String &p_path, String &r_content);

#if defined(__GNUC__)
#define _PRINTF_FORMAT_ATTRIBUTE_1_0 __attribute__((format(printf, 1, 0)))
#define _PRINTF_FORMAT_ATTRIBUTE_1_2 __attribute__((format(printf, 1, 2)))
#else
#define _PRINTF_FORMAT_ATTRIBUTE_1_0
#define _PRINTF_FORMAT_ATTRIBUTE_1_2
#endif

String str_format(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_1_2;
String str_format(const char *p_format, va_list p_list) _PRINTF_FORMAT_ATTRIBUTE_1_0;
char *str_format_new(const char *p_format, ...) _PRINTF_FORMAT_ATTRIBUTE_1_2;
char *str_format_new(const char *p_format, va_list p_list) _PRINTF_FORMAT_ATTRIBUTE_1_0;

#endif // MONO_STRING_UTILS_H
