/**************************************************************************/
/*  zip_io.h                                                              */
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

#ifndef ZIP_IO_H
#define ZIP_IO_H

#include "core/os/file_access.h"

// Not directly used in this header, but assumed available in downstream users
// like platform/*/export/export.cpp. Could be fixed, but probably better to have
// thirdparty includes in as little headers as possible.
#include "thirdparty/minizip/unzip.h"
#include "thirdparty/minizip/zip.h"

void *zipio_open(void *data, const char *p_fname, int mode);
uLong zipio_read(void *data, void *fdata, void *buf, uLong size);
uLong zipio_write(voidpf opaque, voidpf stream, const void *buf, uLong size);

long zipio_tell(voidpf opaque, voidpf stream);
long zipio_seek(voidpf opaque, voidpf stream, uLong offset, int origin);

int zipio_close(voidpf opaque, voidpf stream);

int zipio_testerror(voidpf opaque, voidpf stream);

voidpf zipio_alloc(voidpf opaque, uInt items, uInt size);
void zipio_free(voidpf opaque, voidpf address);

zlib_filefunc_def zipio_create_io_from_file(FileAccess **p_file);

#endif // ZIP_IO_H
