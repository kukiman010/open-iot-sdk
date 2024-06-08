# File naming conventions

The Open IoT SDK uses common conventions for naming files and directories.

## CMake
All CMake files are named following the `CamelCase` naming convention.
### Listing text file
Input files listing directives for the CMake build system are named `CMakeLists.txt`.
### Files and Modules
Files containing CMake code are named starting with a verb e.g. `FetchMcuDriverHal.cmake`.

## Other files and directories
All other files and directories are named following the `snake_case` naming convention.

___
**Note**

Components located in the [`components/`](../../components/) are external repositories populated using CMake FetchContent modules.

To ease recognition of the project added, components root directories are named after the repository they are populated from (albeit in lower case).

E.g.  The CMSIS Version 5 project which lives at https://github.com/ARM-software/CMSIS_5 is fetched from the Open IoT SDK component named [`components/cmsis_5`](../../components/cmsis_5/).

Some projects use hyphens as word separator (`kebab-case`), the Open IoT SDK component shall retain this as it is part of the repository name.

E.g. The Trusted Firmware-M project which lives at https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git is fetched from the Open IoT SDK component named [`components/trusted-firmware-m`](../../components/trusted-firmware-m/).
___
