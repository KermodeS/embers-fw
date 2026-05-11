# ESP-IDF CMake Loop Fix

**Date:** May 2026
**Context:** Blocker between ESP-0 (toolchain setup) and ESP-1 (App Scaffold)

## Symptom

idf.py build enters an infinite "[0/1] Re-running CMake..." loop.
CMake re-configures every ~2 seconds, never reaching ninja compile steps.

## Root cause

System CMake 4.3.2 (installed from the Kitware apt repository) is
incompatible with ESP-IDF v5.5.4. CMake 4.x writes timestamps on
CMakeFiles/4.3.2/CMakeCCompiler.cmake, CMakeCXXCompiler.cmake, and
CMakeSystem.cmake that end up newer than build.ninja on every run.
Ninja sees these as inputs to build.ninja, sees they are newer, and
re-runs CMake. CMake rewrites them. Loop forever.

ESP-IDF v5.5.4 was tested against CMake 3.24-3.30. The bundled CMake
(3.30.2) does not exhibit this behaviour.

## Fix

One-time install of ESP-IDF bundled CMake:

    python3 ~/esp/esp-idf/tools/idf_tools.py install cmake

This installs CMake 3.30.2 to ~/.espressif/tools/cmake/3.30.2/.

After ". ~/esp/esp-idf/export.sh", the bundled cmake is first on PATH:

    $ which cmake
    /home/workstation/.espressif/tools/cmake/3.30.2/bin/cmake
    $ cmake --version
    cmake version 3.30.2

## Reverted patches

The previous troubleshooting session left local modifications in
~/esp/esp-idf/ that targeted symptoms of this mismatch, not the cause.
All have been reverted via git checkout:

- CMakeLists.txt
- components/newlib/project_include.cmake
- components/soc/project_include.cmake
- tools/cmake/toolchain.cmake
- tools/cmake/toolchain_flags.cmake

git status in ~/esp/esp-idf/ now reports clean (apart from the three
expected untracked blob library directories under components/bt/).

## Obsolete artefact

~/build_embers.sh - wrapper script created to work around the loop
via "idf.py reconfigure" + touch. No longer needed. Deleted.

## Verification

Fresh terminal, no source:

    $ which cmake
    /usr/bin/cmake
    $ cmake --version
    cmake version 4.3.2

After ". ~/esp/esp-idf/export.sh":

    $ which cmake
    /home/workstation/.espressif/tools/cmake/3.30.2/bin/cmake
    $ cmake --version
    cmake version 3.30.2

idf.py build on the unmodified hello_world template completed
successfully: embers_fw.bin binary size 0x2e740 bytes.

## Rule going forward

Never patch ESP-IDF internal files. If a future ESP-IDF release fails
to build, the first hypothesis is a tool version mismatch, not a bug
in ESP-IDF CMake logic. Check "which cmake" first.
