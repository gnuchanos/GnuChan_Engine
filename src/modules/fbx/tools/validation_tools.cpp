/**************************************************************************/
/*  validation_tools.cpp                                                  */
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

#include "validation_tools.h"

#ifdef TOOLS_ENABLED

#include "core/print_string.h"
#include "core/ustring.h"

ValidationTracker::Entries *ValidationTracker::entries_singleton = memnew(ValidationTracker::Entries);

// for printing our CSV to dump validation problems of files
// later we can make some agnostic tooling for this but this is fine for the time being.
void ValidationTracker::Entries::add_validation_error(String asset_path, String message) {
	print_error(message);
	// note: implementation is static
	validation_entries[asset_path].push_back(message);
}

#endif // TOOLS_ENABLED
