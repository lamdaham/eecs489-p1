# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src

# Include any dependencies generated for this target.
include src/CMakeFiles/iPerfer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/iPerfer.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/iPerfer.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/iPerfer.dir/flags.make

src/CMakeFiles/iPerfer.dir/codegen:
.PHONY : src/CMakeFiles/iPerfer.dir/codegen

src/CMakeFiles/iPerfer.dir/iPerfer.cpp.o: src/CMakeFiles/iPerfer.dir/flags.make
src/CMakeFiles/iPerfer.dir/iPerfer.cpp.o: iPerfer.cpp
src/CMakeFiles/iPerfer.dir/iPerfer.cpp.o: src/CMakeFiles/iPerfer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/iPerfer.dir/iPerfer.cpp.o"
	cd /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/iPerfer.dir/iPerfer.cpp.o -MF CMakeFiles/iPerfer.dir/iPerfer.cpp.o.d -o CMakeFiles/iPerfer.dir/iPerfer.cpp.o -c /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/iPerfer.cpp

src/CMakeFiles/iPerfer.dir/iPerfer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/iPerfer.dir/iPerfer.cpp.i"
	cd /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/iPerfer.cpp > CMakeFiles/iPerfer.dir/iPerfer.cpp.i

src/CMakeFiles/iPerfer.dir/iPerfer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/iPerfer.dir/iPerfer.cpp.s"
	cd /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/iPerfer.cpp -o CMakeFiles/iPerfer.dir/iPerfer.cpp.s

# Object files for target iPerfer
iPerfer_OBJECTS = \
"CMakeFiles/iPerfer.dir/iPerfer.cpp.o"

# External object files for target iPerfer
iPerfer_EXTERNAL_OBJECTS =

bin/iPerfer: src/CMakeFiles/iPerfer.dir/iPerfer.cpp.o
bin/iPerfer: src/CMakeFiles/iPerfer.dir/build.make
bin/iPerfer: _deps/spdlog-build/libspdlog.a
bin/iPerfer: src/CMakeFiles/iPerfer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/iPerfer"
	cd /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iPerfer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/iPerfer.dir/build: bin/iPerfer
.PHONY : src/CMakeFiles/iPerfer.dir/build

src/CMakeFiles/iPerfer.dir/clean:
	cd /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src && $(CMAKE_COMMAND) -P CMakeFiles/iPerfer.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/iPerfer.dir/clean

src/CMakeFiles/iPerfer.dir/depend:
	cd /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src /Users/ivanlam/Desktop/eecs489/eecs489-p1/cpp/src/src/CMakeFiles/iPerfer.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/CMakeFiles/iPerfer.dir/depend

