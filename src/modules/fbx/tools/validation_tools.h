/**************************************************************************/
/*  validation_tools.h                                                    */
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

#ifndef VALIDATION_TOOLS_H
#define VALIDATION_TOOLS_H

#ifdef TOOLS_ENABLED

#include "core/local_vector.h"
#include "core/map.h"
#include "core/ustring.h"
#include <core/io/json.h>
#include <core/os/file_access.h>
#include <scene/3d/path.h>

class ValidationTracker {
protected:
	struct Entries {
		Map<String, LocalVector<String>> validation_entries = Map<String, LocalVector<String>>();

		// for printing our CSV to dump validation problems of files
		// later we can make some agnostic tooling for this but this is fine for the time being.
		void add_validation_error(String asset_path, String message);
		void print_to_csv() {
			print_verbose("Exporting assset validation log please wait");
			String massive_log_file;

			String csv_header = "file_path, error message, extra data\n";
			massive_log_file += csv_header;

			for (Map<String, LocalVector<String>>::Element *element = validation_entries.front(); element; element = element->next()) {
				for (unsigned int x = 0; x < element->value().size(); x++) {
					const String &line_entry = element->key() + ", " + element->value()[x].c_escape() + "\n";
					massive_log_file += line_entry;
				}
			}

			String path = "asset_validation_errors.csv";
			Error err;
			FileAccess *file = FileAccess::open(path, FileAccess::WRITE, &err);
			if (!file || err) {
				if (file) {
					memdelete(file);
				}
				print_error("ValidationTracker Error - failed to create file - path: %s\n" + path);
				return;
			}

			file->store_string(massive_log_file);
			if (file->get_error() != OK && file->get_error() != ERR_FILE_EOF) {
				print_error("ValidationTracker Error - failed to write to file - path: %s\n" + path);
			}
			file->close();
			memdelete(file);
		}
	};
	// asset path, error messages
	static Entries *entries_singleton;

public:
	static Entries *get_singleton() {
		return entries_singleton;
	}
};

#endif // TOOLS_ENABLED

#endif // VALIDATION_TOOLS_H
