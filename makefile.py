#!/usr/bin/env python3
"""
GnuChanIDE Build Script
Usage:
    py.exe makefile.py                              # Build debug editor
    py.exe makefile.py --release                    # Build release editor
    py.exe makefile.py --clean                      # Clean
    py.exe makefile.py -j8                          # Build with 8 threads
"""

import os
import sys
import subprocess

BUILD_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "game_IDE")
sys.path.insert(0, BUILD_DIR)
from version import major, minor, patch, status as version_status, module_config

SCONS_BASE = [sys.executable, "-m", "SCons", "platform=windows"]

def run_scons(args_list):
    cmd = SCONS_BASE + args_list
    print(f"[BUILD] {' '.join(cmd)}")
    return subprocess.run(cmd, cwd=BUILD_DIR).returncode

def main():
    extra = []
    do_clean = False
    do_release = False

    for arg in sys.argv[1:]:
        if arg == "--release":
            do_release = True
        elif arg == "--clean":
            do_clean = True
        elif arg == "--debug":
            do_release = False
        else:
            extra.append(arg)

    if do_clean:
        run_scons(["--clean"])
        sys.exit(0)

    if do_release:
        ret = run_scons(["target=release_debug", "tools=yes"] + extra)
    else:
        ret = run_scons(["target=debug", "tools=yes"] + extra)

    if ret == 0:
        print(f"[BUILD] SUCCESS.")
    else:
        print(f"[BUILD] FAILED (exit {ret}).")
    sys.exit(ret)

if __name__ == "__main__":
    main()
