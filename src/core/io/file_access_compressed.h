/**************************************************************************/
/*  file_access_compressed.h                                              */
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

#ifndef FILE_ACCESS_COMPRESSED_H
#define FILE_ACCESS_COMPRESSED_H

#include "core/io/compression.h"
#include "core/os/file_access.h"

class FileAccessCompressed : public FileAccess {
	Compression::Mode cmode;
	bool writing;
	uint64_t write_pos;
	uint8_t *write_ptr;
	uint32_t write_buffer_size;
	uint64_t write_max;
	uint32_t block_size;
	mutable bool read_eof;
	mutable bool at_end;

	struct ReadBlock {
		uint32_t csize;
		uint64_t offset;
	};

	mutable Vector<uint8_t> comp_buffer;
	uint8_t *read_ptr;
	mutable uint32_t read_block;
	uint32_t read_block_count;
	mutable uint32_t read_block_size;
	mutable uint64_t read_pos;
	Vector<ReadBlock> read_blocks;
	uint64_t read_total;

	String magic;
	mutable Vector<uint8_t> buffer;
	FileAccess *f;

public:
	void configure(const String &p_magic, Compression::Mode p_mode = Compression::MODE_ZSTD, uint32_t p_block_size = 4096);

	Error open_after_magic(FileAccess *p_base);

	virtual Error _open(const String &p_path, int p_mode_flags); ///< open a file
	virtual void close(); ///< close a file
	virtual bool is_open() const; ///< true when file is open

	virtual String get_path() const; /// returns the path for the current open file
	virtual String get_path_absolute() const; /// returns the absolute path for the current open file

	virtual void seek(uint64_t p_position); ///< seek to a given position
	virtual void seek_end(int64_t p_position = 0); ///< seek from the end of file
	virtual uint64_t get_position() const; ///< get position in the file
	virtual uint64_t get_len() const; ///< get size of the file

	virtual bool eof_reached() const; ///< reading passed EOF

	virtual uint8_t get_8() const; ///< get a byte
	virtual uint64_t get_buffer(uint8_t *p_dst, uint64_t p_length) const;

	virtual Error get_error() const; ///< get last error

	virtual void flush();
	virtual void store_8(uint8_t p_dest); ///< store a byte

	virtual bool file_exists(const String &p_name); ///< return true if a file exists

	virtual uint64_t _get_modified_time(const String &p_file);
	virtual uint32_t _get_unix_permissions(const String &p_file);
	virtual Error _set_unix_permissions(const String &p_file, uint32_t p_permissions);

	FileAccessCompressed();
	virtual ~FileAccessCompressed();
};

#endif // FILE_ACCESS_COMPRESSED_H
