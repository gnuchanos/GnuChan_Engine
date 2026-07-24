/**************************************************************************/
/*  dir_access_windows.h                                                  */
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

#ifndef DIR_ACCESS_WINDOWS_H
#define DIR_ACCESS_WINDOWS_H

#ifdef WINDOWS_ENABLED

#include "core/os/dir_access.h"

/**
*/

struct DirAccessWindowsPrivate;

class DirAccessWindows : public DirAccess {
	enum {
		MAX_DRIVES = 26
	};

	DirAccessWindowsPrivate *p;
	/* Windows stuff */

	char drives[MAX_DRIVES]; // a-z:
	int drive_count;

	String current_dir;

	bool _cisdir;
	bool _cishidden;

public:
	virtual Error list_dir_begin(); ///< This starts dir listing
	virtual String get_next();
	virtual bool current_is_dir() const;
	virtual bool current_is_hidden() const;
	virtual void list_dir_end(); ///<

	virtual int get_drive_count();
	virtual String get_drive(int p_drive);

	virtual Error change_dir(String p_dir); ///< can be relative or absolute, return false on success
	virtual String get_current_dir(); ///< return current dir location
	virtual String get_current_dir_without_drive();

	virtual bool file_exists(String p_file);
	virtual bool dir_exists(String p_dir);

	virtual Error make_dir(String p_dir);

	virtual Error rename(String p_path, String p_new_path);
	virtual Error remove(String p_path);

	virtual bool is_link(String p_file) { return false; };
	virtual String read_link(String p_file) { return p_file; };
	virtual Error create_link(String p_source, String p_target) { return FAILED; };

	uint64_t get_space_left();

	virtual String get_filesystem_type() const;

	DirAccessWindows();
	~DirAccessWindows();
};

#endif // WINDOWS_ENABLED

#endif // DIR_ACCESS_WINDOWS_H
