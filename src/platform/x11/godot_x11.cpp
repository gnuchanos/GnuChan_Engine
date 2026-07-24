/**************************************************************************/
/*  godot_x11.cpp                                                         */
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

#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "main/main.h"
#include "os_x11.h"

// For export templates, add a section; the exporter will patch it to enclose
// the data appended to the executable (bundled PCK).
#if !defined(TOOLS_ENABLED) && defined(__GNUC__)
static const char dummy[8] __attribute__((section("pck"), used)) = { 0 };

// Dummy function to prevent LTO from discarding "pck" section.
extern "C" const char *pck_section_dummy_call() __attribute__((used));
extern "C" const char *pck_section_dummy_call() {
	return &dummy[0];
}
#endif

int main(int argc, char *argv[]) {
	OS_X11 os;

	setlocale(LC_CTYPE, "");

	char *cwd = (char *)malloc(PATH_MAX);
	ERR_FAIL_COND_V(!cwd, ERR_OUT_OF_MEMORY);
	char *ret = getcwd(cwd, PATH_MAX);

	Error err = Main::setup(argv[0], argc - 1, &argv[1]);
	if (err != OK) {
		free(cwd);

		if (err == ERR_HELP) { // Returned by --help and --version, so success.
			return 0;
		}
		return 255;
	}

	if (Main::start()) {
		os.run(); // it is actually the OS that decides how to run
	}
	Main::cleanup();

	if (ret) { // Previous getcwd was successful
		if (chdir(cwd) != 0) {
			ERR_PRINT("Couldn't return to previous working directory.");
		}
	}
	free(cwd);

	return os.get_exit_code();
}
