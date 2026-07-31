#!/usr/bin/env python3
"""
GnuChanIDE Build Script (Windows)
Usage:
    py.exe makefile.py                      # Debug editor build (default -j{cpu_count})
    py.exe makefile.py --release            # Release_debug editor build
    py.exe makefile.py --clean              # Clean build artifacts
    py.exe makefile.py -j8                  # Override thread count

Output: src/bin/GnuChanIDE.exe (Godot 3.x SCons convention)
"""

import os
import sys
import subprocess

ROOT = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR = os.path.join(ROOT, "src")
BIN_DIR = os.path.join(BUILD_DIR, "bin")
EXE = os.path.join(BIN_DIR, "GnuChanIDE.exe")

sys.path.insert(0, BUILD_DIR)
from version import major, minor, patch, status as version_status

VERSION = f"{major}.{minor}.{patch}-{version_status}"

# Bullet kaynakları bu depoda eksik; modülü devre dışı bırak
# (referansları kaldırır, eksik dosya hatasını önler)
SCONS_BASE = [sys.executable, "-m", "SCons", "platform=windows", "module_bullet_enabled=no"]


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

    print(f"[BUILD] GnuChanIDE {VERSION}")

    # -j belirtilmediyse tüm çekirdekleri kullan
    if not any(a.startswith("-j") for a in extra):
        extra.append(f"-j{os.cpu_count() or 2}")

    if do_clean:
        return run_scons(["--clean"])

    if do_release:
        ret = run_scons(["target=release_debug", "tools=yes"] + extra)
    else:
        ret = run_scons(["target=debug", "tools=yes"] + extra)

    if ret == 0:
        print(f"[BUILD] SUCCESS -> {EXE}")
    else:
        print(f"[BUILD] FAILED (exit {ret}).")
    return ret


if __name__ == "__main__":
    sys.exit(main())
