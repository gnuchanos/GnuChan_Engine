/**************************************************************************/
/*  eq_filter.h                                                           */
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

#ifndef EQ_FILTER_H
#define EQ_FILTER_H

#include "core/typedefs.h"
#include "core/vector.h"

/**
*/

class EQ {
public:
	enum Preset {

		PRESET_6_BANDS,
		PRESET_8_BANDS,
		PRESET_10_BANDS,
		PRESET_21_BANDS,
		PRESET_31_BANDS
	};

	class BandProcess {
		friend class EQ;
		float c1, c2, c3;
		struct History {
			float a1, a2, a3;
			float b1, b2, b3;

		} history;

	public:
		inline void process_one(float &p_data);

		BandProcess();
	};

private:
	struct Band {
		float freq;
		float c1, c2, c3;
	};

	Vector<Band> band;

	float mix_rate;

	void recalculate_band_coefficients();

public:
	void set_mix_rate(float p_mix_rate);

	int get_band_count() const;
	void set_preset_band_mode(Preset p_preset);
	void set_bands(const Vector<float> &p_bands);
	BandProcess get_band_processor(int p_band) const;
	float get_band_frequency(int p_band);

	EQ();
	~EQ();
};

/* Inline Function */

inline void EQ::BandProcess::process_one(float &p_data) {
	history.a1 = p_data;

	history.b1 = c1 * (history.a1 - history.a3) + c3 * history.b2 - c2 * history.b3;

	p_data = history.b1;

	history.a3 = history.a2;
	history.a2 = history.a1;
	history.b3 = history.b2;
	history.b2 = history.b1;
}

#endif // EQ_FILTER_H
