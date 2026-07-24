/**************************************************************************/
/*  pck_packer.h                                                          */
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

#ifndef PCK_PACKER_H
#define PCK_PACKER_H

#include "core/reference.h"

class FileAccess;

class PCKPacker : public Reference {
	GDCLASS(PCKPacker, Reference);

	FileAccess *file;
	int alignment;

	static void _bind_methods();

	struct File {
		String path;
		String src_path;
		uint64_t size;
		uint64_t offset_offset;
	};
	Vector<File> files;

public:
	Error pck_start(const String &p_file, int p_alignment = 0);
	Error add_file(const String &p_file, const String &p_src);
	Error flush(bool p_verbose = false);

	PCKPacker();
	~PCKPacker();
};

#endif // PCK_PACKER_H
