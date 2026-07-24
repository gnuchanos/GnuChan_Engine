/**************************************************************************/
/*  dir_access_unix.h                                                     */
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

#ifndef DIR_ACCESS_UNIX_H
#define DIR_ACCESS_UNIX_H

#if defined(UNIX_ENABLED) || defined(LIBC_FILEIO_ENABLED)

#include "core/os/dir_access.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class DirAccessUnix : public DirAccess {
	DIR *dir_stream;

	bool _cisdir;
	bool _cishidden;

protected:
	String current_dir;
	virtual String fix_unicode_name(const char *p_name) const { return String::utf8(p_name); }
	virtual bool is_hidden(const String &p_name);

public:
	virtual Error list_dir_begin(); ///< This starts dir listing
	virtual String get_next();
	virtual bool current_is_dir() const;
	virtual bool current_is_hidden() const;

	virtual void list_dir_end(); ///<

	virtual int get_drive_count();
	virtual String get_drive(int p_drive);
	virtual int get_current_drive();
	virtual bool drives_are_shortcuts();

	virtual Error change_dir(String p_dir); ///< can be relative or absolute, return false on success
	virtual String get_current_dir(); ///< return current dir location
	virtual Error make_dir(String p_dir);

	virtual bool file_exists(String p_file);
	virtual bool dir_exists(String p_dir);

	virtual uint64_t get_modified_time(String p_file);

	virtual Error rename(String p_path, String p_new_path);
	virtual Error remove(String p_path);

	virtual bool is_link(String p_file);
	virtual String read_link(String p_file);
	virtual Error create_link(String p_source, String p_target);

	virtual uint64_t get_space_left();

	virtual String get_filesystem_type() const;

	DirAccessUnix();
	~DirAccessUnix();
};

#endif // UNIX_ENABLED || LIBC_FILEIO_ENABLED

#endif // DIR_ACCESS_UNIX_H
