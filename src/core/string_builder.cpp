/**************************************************************************/
/*  string_builder.cpp                                                    */
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

#include "string_builder.h"

#include <string.h>

StringBuilder &StringBuilder::append(const String &p_string) {
	if (p_string == String()) {
		return *this;
	}

	strings.push_back(p_string);
	appended_strings.push_back(-1);

	string_length += p_string.length();

	return *this;
}

StringBuilder &StringBuilder::append(const char *p_cstring) {
	int32_t len = strlen(p_cstring);

	c_strings.push_back(p_cstring);
	appended_strings.push_back(len);

	string_length += len;

	return *this;
}

String StringBuilder::as_string() const {
	if (string_length == 0) {
		return "";
	}

	CharType *buffer = memnew_arr(CharType, string_length);

	int current_position = 0;

	int godot_string_elem = 0;
	int c_string_elem = 0;

	for (int i = 0; i < appended_strings.size(); i++) {
		if (appended_strings[i] == -1) {
			// GnuChan string
			const String &s = strings[godot_string_elem];

			memcpy(buffer + current_position, s.ptr(), s.length() * sizeof(CharType));

			current_position += s.length();

			godot_string_elem++;
		} else {
			const char *s = c_strings[c_string_elem];

			for (int32_t j = 0; j < appended_strings[i]; j++) {
				buffer[current_position + j] = s[j];
			}

			current_position += appended_strings[i];

			c_string_elem++;
		}
	}

	String final_string = String(buffer, string_length);

	memdelete_arr(buffer);

	return final_string;
}
