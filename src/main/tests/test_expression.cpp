/**************************************************************************/
/*  test_expression.cpp                                                   */
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

#include "test_expression.h"

#include "core/math/expression.h"
#include "core/os/os.h"

#define CHECK_MESSAGE(X, msg)                                      \
	if (!(X)) {                                                    \
		OS::get_singleton()->print("\tFAIL at %s: %s\n", #X, msg); \
		return false;                                              \
	} else {                                                       \
		OS::get_singleton()->print("\tPASS\n");                    \
	}

namespace TestExpression {

bool floating_point_notation() {
	OS::get_singleton()->print("\n\nTest 1: Floating-point notation\n");

	Expression expression;

	CHECK_MESSAGE(
			expression.parse("2.") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), 2.0),
			"The expression should return the expected result.");

	CHECK_MESSAGE(
			expression.parse("(2.)") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), 2.0),
			"The expression should return the expected result.");

	CHECK_MESSAGE(
			expression.parse(".3") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), 0.3),
			"The expression should return the expected result.");

	CHECK_MESSAGE(
			expression.parse("2.+5.") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), 7.0),
			"The expression should return the expected result.");

	CHECK_MESSAGE(
			expression.parse(".3-.8") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), -0.5),
			"The expression should return the expected result.");

	CHECK_MESSAGE(
			expression.parse("2.+.2") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), 2.2),
			"The expression should return the expected result.");

	CHECK_MESSAGE(
			expression.parse(".0*0.") == OK,
			"The expression should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(expression.execute(Array()), 0.0),
			"The expression should return the expected result.");

	return true;
}

typedef bool (*TestFunc)();

TestFunc test_funcs[] = {
	floating_point_notation,
	nullptr
};

MainLoop *test() {
	int count = 0;
	int passed = 0;

	while (true) {
		if (!test_funcs[count]) {
			break;
		}
		bool pass = test_funcs[count]();
		if (pass) {
			passed++;
		}
		OS::get_singleton()->print("\t%s\n", pass ? "PASS" : "FAILED");

		count++;
	}

	OS::get_singleton()->print("\n\n\n");
	OS::get_singleton()->print("*************\n");
	OS::get_singleton()->print("***TOTALS!***\n");
	OS::get_singleton()->print("*************\n");

	OS::get_singleton()->print("Passed %i of %i tests\n", passed, count);

	return nullptr;
}
} // namespace TestExpression
