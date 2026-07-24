/**************************************************************************/
/*  text_file.h                                                           */
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

#ifndef TEXT_FILE_H
#define TEXT_FILE_H

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"

class TextFile : public Resource {
	GDCLASS(TextFile, Resource);

private:
	String text;
	String path;

public:
	virtual bool has_text() const;
	virtual String get_text() const;
	virtual void set_text(const String &p_code);
	virtual void reload_from_file();

	void set_file_path(const String &p_path) { path = p_path; }
	Error load_text(const String &p_path);
};

#endif // TEXT_FILE_H
