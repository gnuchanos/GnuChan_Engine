/**************************************************************************/
/*  macros.h                                                              */
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

#ifndef MONO_MACROS_H
#define MONO_MACROS_H

#define _GD_VARNAME_CONCAT_B_(m_ignore, m_name) m_name
#define _GD_VARNAME_CONCAT_A_(m_a, m_b, m_c) _GD_VARNAME_CONCAT_B_(hello there, m_a##m_b##m_c)
#define _GD_VARNAME_CONCAT_(m_a, m_b, m_c) _GD_VARNAME_CONCAT_A_(m_a, m_b, m_c)
#define GD_UNIQUE_NAME(m_name) _GD_VARNAME_CONCAT_(m_name, _, __COUNTER__)

// static assert
// TODO: Get rid of this macro once we upgrade to C++11

#ifdef __cpp_static_assert
#define GD_STATIC_ASSERT(m_cond) static_assert((m_cond), "Condition '" #m_cond "' failed")
#else
#define GD_STATIC_ASSERT(m_cond) typedef int GD_UNIQUE_NAME(godot_static_assert)[((m_cond) ? 1 : -1)]
#endif

// final
// TODO: Get rid of this macro once we upgrade to C++11

#if (__cplusplus >= 201103L)
#define GD_FINAL final
#else
#define GD_FINAL
#endif

// noreturn
// TODO: Get rid of this macro once we upgrade to C++11

#if (__cplusplus >= 201103L)
#define GD_NORETURN [[noreturn]]
#elif defined(__GNUC__)
#define GD_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define GD_NORETURN __declspec(noreturn)
#else
#define GD_NORETURN
#pragma message "Macro GD_NORETURN will have no effect"
#endif

// unreachable

#if defined(_MSC_VER)
#define GD_UNREACHABLE() __assume(0)
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 405
#define GD_UNREACHABLE() __builtin_unreachable()
#else
#define GD_UNREACHABLE() \
	CRASH_NOW();         \
	do {                 \
	} while (true);
#endif

namespace gdmono {

template <typename F>
struct ScopeExit {
	ScopeExit(F p_exit_func) :
			exit_func(p_exit_func) {}
	~ScopeExit() { exit_func(); }
	F exit_func;
};

class ScopeExitAux {
public:
	template <typename F>
	ScopeExit<F> operator+(F p_exit_func) { return ScopeExit<F>(p_exit_func); }
};

} // namespace gdmono

#define SCOPE_EXIT \
	auto GD_UNIQUE_NAME(gd_scope_exit) = gdmono::ScopeExitAux() + [=]() -> void

#endif // MONO_MACROS_H
