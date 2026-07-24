/**************************************************************************/
/*  random_number_generator.h                                             */
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

#ifndef RANDOM_NUMBER_GENERATOR_H
#define RANDOM_NUMBER_GENERATOR_H

#include "core/math/random_pcg.h"
#include "core/reference.h"

class RandomNumberGenerator : public Reference {
	GDCLASS(RandomNumberGenerator, Reference);

protected:
	RandomPCG randbase;

	static void _bind_methods();

public:
	_FORCE_INLINE_ void set_seed(uint64_t seed) { randbase.seed(seed); }

	_FORCE_INLINE_ uint64_t get_seed() { return randbase.get_seed(); }

	_FORCE_INLINE_ void set_state(uint64_t p_state) { randbase.set_state(p_state); }

	_FORCE_INLINE_ uint64_t get_state() const { return randbase.get_state(); }

	_FORCE_INLINE_ void randomize() { randbase.randomize(); }

	_FORCE_INLINE_ uint32_t randi() { return randbase.rand(); }

	_FORCE_INLINE_ real_t randf() { return randbase.randf(); }

	_FORCE_INLINE_ real_t randf_range(real_t from, real_t to) { return randbase.random(from, to); }

	_FORCE_INLINE_ real_t randfn(real_t mean = 0.0, real_t deviation = 1.0) { return randbase.randfn(mean, deviation); }

	_FORCE_INLINE_ int randi_range(int from, int to) {
		unsigned int ret = randbase.rand();
		if (to < from) {
			return ret % (from - to + 1) + to;
		} else {
			return ret % (to - from + 1) + from;
		}
	}

	RandomNumberGenerator();
};

#endif // RANDOM_NUMBER_GENERATOR_H
