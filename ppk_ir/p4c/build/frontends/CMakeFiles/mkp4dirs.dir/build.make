# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aa/ppk_sw/ppk/p4c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aa/ppk_sw/ppk/p4c/build

# Utility rule file for mkp4dirs.

# Include the progress variables for this target.
include frontends/CMakeFiles/mkp4dirs.dir/progress.make

frontends/CMakeFiles/mkp4dirs:
	cd /home/aa/ppk_sw/ppk/p4c/build/frontends && /usr/bin/cmake -E make_directory /home/aa/ppk_sw/ppk/p4c/build/frontends/parsers/p4

mkp4dirs: frontends/CMakeFiles/mkp4dirs
mkp4dirs: frontends/CMakeFiles/mkp4dirs.dir/build.make

.PHONY : mkp4dirs

# Rule to build all files generated by this target.
frontends/CMakeFiles/mkp4dirs.dir/build: mkp4dirs

.PHONY : frontends/CMakeFiles/mkp4dirs.dir/build

frontends/CMakeFiles/mkp4dirs.dir/clean:
	cd /home/aa/ppk_sw/ppk/p4c/build/frontends && $(CMAKE_COMMAND) -P CMakeFiles/mkp4dirs.dir/cmake_clean.cmake
.PHONY : frontends/CMakeFiles/mkp4dirs.dir/clean

frontends/CMakeFiles/mkp4dirs.dir/depend:
	cd /home/aa/ppk_sw/ppk/p4c/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aa/ppk_sw/ppk/p4c /home/aa/ppk_sw/ppk/p4c/frontends /home/aa/ppk_sw/ppk/p4c/build /home/aa/ppk_sw/ppk/p4c/build/frontends /home/aa/ppk_sw/ppk/p4c/build/frontends/CMakeFiles/mkp4dirs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : frontends/CMakeFiles/mkp4dirs.dir/depend

