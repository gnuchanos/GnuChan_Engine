/**************************************************************************/
/*  zip_io.cpp                                                            */
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

#include "zip_io.h"

void *zipio_open(void *data, const char *p_fname, int mode) {
	FileAccess *&f = *(FileAccess **)data;

	String fname;
	fname.parse_utf8(p_fname);

	if (mode & ZLIB_FILEFUNC_MODE_WRITE) {
		f = FileAccess::open(fname, FileAccess::WRITE);
	} else {
		f = FileAccess::open(fname, FileAccess::READ);
	}

	if (!f) {
		return nullptr;
	}

	return data;
}

uLong zipio_read(void *data, void *fdata, void *buf, uLong size) {
	FileAccess *f = *(FileAccess **)data;
	return f->get_buffer((uint8_t *)buf, size);
}

uLong zipio_write(voidpf opaque, voidpf stream, const void *buf, uLong size) {
	FileAccess *f = *(FileAccess **)opaque;
	f->store_buffer((uint8_t *)buf, size);
	return size;
}

long zipio_tell(voidpf opaque, voidpf stream) {
	FileAccess *f = *(FileAccess **)opaque;
	return f->get_position();
}

long zipio_seek(voidpf opaque, voidpf stream, uLong offset, int origin) {
	FileAccess *f = *(FileAccess **)opaque;

	uint64_t pos = offset;
	switch (origin) {
		case ZLIB_FILEFUNC_SEEK_CUR:
			pos = f->get_position() + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_END:
			pos = f->get_len() + offset;
			break;
		default:
			break;
	};

	f->seek(pos);
	return 0;
}

int zipio_close(voidpf opaque, voidpf stream) {
	FileAccess *&f = *(FileAccess **)opaque;
	if (f) {
		f->close();
		memdelete(f);
		f = nullptr;
	}
	return 0;
}

int zipio_testerror(voidpf opaque, voidpf stream) {
	FileAccess *f = *(FileAccess **)opaque;
	return (f && f->get_error() != OK) ? 1 : 0;
}

voidpf zipio_alloc(voidpf opaque, uInt items, uInt size) {
	voidpf ptr = memalloc(items * size);
	memset(ptr, 0, items * size);
	return ptr;
}

void zipio_free(voidpf opaque, voidpf address) {
	memfree(address);
}

zlib_filefunc_def zipio_create_io_from_file(FileAccess **p_file) {
	zlib_filefunc_def io;
	io.opaque = p_file;
	io.zopen_file = zipio_open;
	io.zread_file = zipio_read;
	io.zwrite_file = zipio_write;
	io.ztell_file = zipio_tell;
	io.zseek_file = zipio_seek;
	io.zclose_file = zipio_close;
	io.zerror_file = zipio_testerror;
	io.alloc_mem = zipio_alloc;
	io.free_mem = zipio_free;
	return io;
}
