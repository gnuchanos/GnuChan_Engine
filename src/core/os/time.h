/**************************************************************************/
/*  time.h                                                                */
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

#ifndef TIME_H
#define TIME_H

#include "core/class_db.h"

// This Time class conforms with as many of the ISO 8601 standards as possible.
// * As per ISO 8601:2004 4.3.2.1, all dates follow the Proleptic Gregorian
//   calendar. As such, the day before 1582-10-15 is 1582-10-14, not 1582-10-04.
//   See: https://en.wikipedia.org/wiki/Proleptic_Gregorian_calendar
// * As per ISO 8601:2004 3.4.2 and 4.1.2.4, the year before 1 AD (aka 1 BC)
//   is number "0", with the year before that (2 BC) being "-1", etc.
// Conversion methods assume "the same timezone", and do not handle DST.
// Leap seconds are not handled, they must be done manually if desired.
// Suffixes such as "Z" are not handled, you need to strip them away manually.

class Time : public Object {
	GDCLASS(Time, Object);
	static void _bind_methods();
	static Time *singleton;

public:
	static Time *get_singleton();

	enum Month {
		/// Start at 1 to follow Windows SYSTEMTIME structure
		/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724950(v=vs.85).aspx
		MONTH_JANUARY = 1,
		MONTH_FEBRUARY,
		MONTH_MARCH,
		MONTH_APRIL,
		MONTH_MAY,
		MONTH_JUNE,
		MONTH_JULY,
		MONTH_AUGUST,
		MONTH_SEPTEMBER,
		MONTH_OCTOBER,
		MONTH_NOVEMBER,
		MONTH_DECEMBER,
	};

	enum Weekday : uint8_t {
		WEEKDAY_SUNDAY,
		WEEKDAY_MONDAY,
		WEEKDAY_TUESDAY,
		WEEKDAY_WEDNESDAY,
		WEEKDAY_THURSDAY,
		WEEKDAY_FRIDAY,
		WEEKDAY_SATURDAY,
	};

	// Methods that convert times.
	Dictionary get_datetime_dict_from_unix_time(int64_t p_unix_time_val) const;
	Dictionary get_date_dict_from_unix_time(int64_t p_unix_time_val) const;
	Dictionary get_time_dict_from_unix_time(int64_t p_unix_time_val) const;
	String get_datetime_string_from_unix_time(int64_t p_unix_time_val, bool p_use_space = false) const;
	String get_date_string_from_unix_time(int64_t p_unix_time_val) const;
	String get_time_string_from_unix_time(int64_t p_unix_time_val) const;
	Dictionary get_datetime_dict_from_datetime_string(String p_datetime, bool p_weekday = true) const;
	String get_datetime_string_from_datetime_dict(const Dictionary p_datetime, bool p_use_space = false) const;
	int64_t get_unix_time_from_datetime_dict(const Dictionary p_datetime) const;
	int64_t get_unix_time_from_datetime_string(String p_datetime) const;
	String get_offset_string_from_offset_minutes(int64_t p_offset_minutes) const;

	// Methods that get information from OS.
	Dictionary get_datetime_dict_from_system(bool p_utc = false) const;
	Dictionary get_date_dict_from_system(bool p_utc = false) const;
	Dictionary get_time_dict_from_system(bool p_utc = false) const;
	String get_datetime_string_from_system(bool p_utc = false, bool p_use_space = false) const;
	String get_date_string_from_system(bool p_utc = false) const;
	String get_time_string_from_system(bool p_utc = false) const;
	Dictionary get_time_zone_from_system() const;
	double get_unix_time_from_system() const;
	uint64_t get_ticks_msec() const;
	uint64_t get_ticks_usec() const;

	Time();
	virtual ~Time();
};

#endif // TIME_H
