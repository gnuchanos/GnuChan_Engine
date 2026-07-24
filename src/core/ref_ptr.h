/**************************************************************************/
/*  ref_ptr.h                                                             */
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

#ifndef REF_PTR_H
#define REF_PTR_H

/**
 * This class exists to workaround a limitation in C++ but keep the design OK.
 * It's basically an opaque container of a Reference reference, so Variant can use it.
*/

#include "core/rid.h"

class RefPtr {
	enum {

		DATASIZE = sizeof(void *) //*4 -ref was shrunk
	};

	mutable char data[DATASIZE]; // too much probably, virtual class + pointer
public:
	bool is_null() const;
	void operator=(const RefPtr &p_other);
	bool operator==(const RefPtr &p_other) const;
	bool operator!=(const RefPtr &p_other) const;
	RID get_rid() const;
	void unref();
	_FORCE_INLINE_ void *get_data() const { return data; }
	RefPtr(const RefPtr &p_other);
	RefPtr();
	~RefPtr();
};

#endif // REF_PTR_H
