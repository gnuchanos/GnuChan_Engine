/**************************************************************************/
/*  arguments_vector.h                                                    */
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

#ifndef ARGUMENTS_VECTOR_H
#define ARGUMENTS_VECTOR_H

#include "core/os/memory.h"

template <typename T, int POOL_SIZE = 5>
struct ArgumentsVector {
private:
	T pool[POOL_SIZE];
	T *_ptr;
	int size;

	ArgumentsVector() = delete;
	ArgumentsVector(const ArgumentsVector &) = delete;

public:
	T *ptr() { return _ptr; }
	T &get(int p_idx) { return _ptr[p_idx]; }
	void set(int p_idx, const T &p_value) { _ptr[p_idx] = p_value; }

	explicit ArgumentsVector(int p_size) :
			size(p_size) {
		if (p_size <= POOL_SIZE) {
			_ptr = pool;
		} else {
			_ptr = memnew_arr(T, p_size);
		}
	}

	~ArgumentsVector() {
		if (size > POOL_SIZE) {
			memdelete_arr(_ptr);
		}
	}
};

#endif // ARGUMENTS_VECTOR_H
