# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pc/ComNet-Sponge

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pc/ComNet-Sponge/apps

# Utility rule file for check_lab4.

# Include any custom commands dependencies for this target.
include CMakeFiles/check_lab4.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/check_lab4.dir/progress.make

CMakeFiles/check_lab4:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/pc/ComNet-Sponge/apps/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Testing the TCP connection..."
	/home/pc/ComNet-Sponge/tun.sh check 144 145
	/usr/bin/ctest --output-on-failure --timeout 10 -R ^t_

check_lab4: CMakeFiles/check_lab4
check_lab4: CMakeFiles/check_lab4.dir/build.make
.PHONY : check_lab4

# Rule to build all files generated by this target.
CMakeFiles/check_lab4.dir/build: check_lab4
.PHONY : CMakeFiles/check_lab4.dir/build

CMakeFiles/check_lab4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/check_lab4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/check_lab4.dir/clean

CMakeFiles/check_lab4.dir/depend:
	cd /home/pc/ComNet-Sponge/apps && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pc/ComNet-Sponge /home/pc/ComNet-Sponge /home/pc/ComNet-Sponge/apps /home/pc/ComNet-Sponge/apps /home/pc/ComNet-Sponge/apps/CMakeFiles/check_lab4.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/check_lab4.dir/depend

