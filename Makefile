# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_COMMAND = /home/peter/bin/miniconda3/lib/python3.10/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/peter/bin/miniconda3/lib/python3.10/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/peter/work/secure-settlement-reporting

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/peter/work/secure-settlement-reporting

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/home/peter/bin/miniconda3/lib/python3.10/site-packages/cmake/data/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/home/peter/bin/miniconda3/lib/python3.10/site-packages/cmake/data/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/peter/work/secure-settlement-reporting/CMakeFiles /home/peter/work/secure-settlement-reporting//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/peter/work/secure-settlement-reporting/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named ssrep

# Build rule for target.
ssrep: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 ssrep
.PHONY : ssrep

# fast build rule for target.
ssrep/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ssrep.dir/build.make CMakeFiles/ssrep.dir/build
.PHONY : ssrep/fast

#=============================================================================
# Target rules for targets named parser

# Build rule for target.
parser: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 parser
.PHONY : parser

# fast build rule for target.
parser/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser.dir/build.make CMakeFiles/parser.dir/build
.PHONY : parser/fast

test/parser.o: test/parser.cpp.o
.PHONY : test/parser.o

# target to build an object file
test/parser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser.dir/build.make CMakeFiles/parser.dir/test/parser.cpp.o
.PHONY : test/parser.cpp.o

test/parser.i: test/parser.cpp.i
.PHONY : test/parser.i

# target to preprocess a source file
test/parser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser.dir/build.make CMakeFiles/parser.dir/test/parser.cpp.i
.PHONY : test/parser.cpp.i

test/parser.s: test/parser.cpp.s
.PHONY : test/parser.s

# target to generate assembly for a file
test/parser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/parser.dir/build.make CMakeFiles/parser.dir/test/parser.cpp.s
.PHONY : test/parser.cpp.s

test/ssrep.o: test/ssrep.cpp.o
.PHONY : test/ssrep.o

# target to build an object file
test/ssrep.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ssrep.dir/build.make CMakeFiles/ssrep.dir/test/ssrep.cpp.o
.PHONY : test/ssrep.cpp.o

test/ssrep.i: test/ssrep.cpp.i
.PHONY : test/ssrep.i

# target to preprocess a source file
test/ssrep.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ssrep.dir/build.make CMakeFiles/ssrep.dir/test/ssrep.cpp.i
.PHONY : test/ssrep.cpp.i

test/ssrep.s: test/ssrep.cpp.s
.PHONY : test/ssrep.s

# target to generate assembly for a file
test/ssrep.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ssrep.dir/build.make CMakeFiles/ssrep.dir/test/ssrep.cpp.s
.PHONY : test/ssrep.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... parser"
	@echo "... ssrep"
	@echo "... test/parser.o"
	@echo "... test/parser.i"
	@echo "... test/parser.s"
	@echo "... test/ssrep.o"
	@echo "... test/ssrep.i"
	@echo "... test/ssrep.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

