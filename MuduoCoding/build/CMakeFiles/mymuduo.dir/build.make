# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/Mortal/MuduoProject/Muduo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/Mortal/MuduoProject/build

# Include any dependencies generated for this target.
include CMakeFiles/mymuduo.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mymuduo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mymuduo.dir/flags.make

CMakeFiles/mymuduo.dir/DefaultPoll.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/DefaultPoll.o: /home/Mortal/MuduoProject/Muduo/DefaultPoll.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/DefaultPoll.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/DefaultPoll.o -c /home/Mortal/MuduoProject/Muduo/DefaultPoll.cc

CMakeFiles/mymuduo.dir/DefaultPoll.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/DefaultPoll.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/DefaultPoll.cc > CMakeFiles/mymuduo.dir/DefaultPoll.i

CMakeFiles/mymuduo.dir/DefaultPoll.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/DefaultPoll.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/DefaultPoll.cc -o CMakeFiles/mymuduo.dir/DefaultPoll.s

CMakeFiles/mymuduo.dir/DefaultPoll.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/DefaultPoll.o.requires

CMakeFiles/mymuduo.dir/DefaultPoll.o.provides: CMakeFiles/mymuduo.dir/DefaultPoll.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/DefaultPoll.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/DefaultPoll.o.provides

CMakeFiles/mymuduo.dir/DefaultPoll.o.provides.build: CMakeFiles/mymuduo.dir/DefaultPoll.o

CMakeFiles/mymuduo.dir/EventLoop.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/EventLoop.o: /home/Mortal/MuduoProject/Muduo/EventLoop.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/EventLoop.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/EventLoop.o -c /home/Mortal/MuduoProject/Muduo/EventLoop.cc

CMakeFiles/mymuduo.dir/EventLoop.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/EventLoop.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/EventLoop.cc > CMakeFiles/mymuduo.dir/EventLoop.i

CMakeFiles/mymuduo.dir/EventLoop.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/EventLoop.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/EventLoop.cc -o CMakeFiles/mymuduo.dir/EventLoop.s

CMakeFiles/mymuduo.dir/EventLoop.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/EventLoop.o.requires

CMakeFiles/mymuduo.dir/EventLoop.o.provides: CMakeFiles/mymuduo.dir/EventLoop.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/EventLoop.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/EventLoop.o.provides

CMakeFiles/mymuduo.dir/EventLoop.o.provides.build: CMakeFiles/mymuduo.dir/EventLoop.o

CMakeFiles/mymuduo.dir/Channel.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/Channel.o: /home/Mortal/MuduoProject/Muduo/Channel.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/Channel.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/Channel.o -c /home/Mortal/MuduoProject/Muduo/Channel.cc

CMakeFiles/mymuduo.dir/Channel.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/Channel.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/Channel.cc > CMakeFiles/mymuduo.dir/Channel.i

CMakeFiles/mymuduo.dir/Channel.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/Channel.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/Channel.cc -o CMakeFiles/mymuduo.dir/Channel.s

CMakeFiles/mymuduo.dir/Channel.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/Channel.o.requires

CMakeFiles/mymuduo.dir/Channel.o.provides: CMakeFiles/mymuduo.dir/Channel.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/Channel.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/Channel.o.provides

CMakeFiles/mymuduo.dir/Channel.o.provides.build: CMakeFiles/mymuduo.dir/Channel.o

CMakeFiles/mymuduo.dir/TcpServer.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/TcpServer.o: /home/Mortal/MuduoProject/Muduo/TcpServer.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/TcpServer.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/TcpServer.o -c /home/Mortal/MuduoProject/Muduo/TcpServer.cc

CMakeFiles/mymuduo.dir/TcpServer.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/TcpServer.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/TcpServer.cc > CMakeFiles/mymuduo.dir/TcpServer.i

CMakeFiles/mymuduo.dir/TcpServer.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/TcpServer.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/TcpServer.cc -o CMakeFiles/mymuduo.dir/TcpServer.s

CMakeFiles/mymuduo.dir/TcpServer.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/TcpServer.o.requires

CMakeFiles/mymuduo.dir/TcpServer.o.provides: CMakeFiles/mymuduo.dir/TcpServer.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/TcpServer.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/TcpServer.o.provides

CMakeFiles/mymuduo.dir/TcpServer.o.provides.build: CMakeFiles/mymuduo.dir/TcpServer.o

CMakeFiles/mymuduo.dir/InetAddress.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/InetAddress.o: /home/Mortal/MuduoProject/Muduo/InetAddress.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/InetAddress.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/InetAddress.o -c /home/Mortal/MuduoProject/Muduo/InetAddress.cc

CMakeFiles/mymuduo.dir/InetAddress.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/InetAddress.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/InetAddress.cc > CMakeFiles/mymuduo.dir/InetAddress.i

CMakeFiles/mymuduo.dir/InetAddress.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/InetAddress.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/InetAddress.cc -o CMakeFiles/mymuduo.dir/InetAddress.s

CMakeFiles/mymuduo.dir/InetAddress.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/InetAddress.o.requires

CMakeFiles/mymuduo.dir/InetAddress.o.provides: CMakeFiles/mymuduo.dir/InetAddress.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/InetAddress.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/InetAddress.o.provides

CMakeFiles/mymuduo.dir/InetAddress.o.provides.build: CMakeFiles/mymuduo.dir/InetAddress.o

CMakeFiles/mymuduo.dir/Poller.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/Poller.o: /home/Mortal/MuduoProject/Muduo/Poller.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/Poller.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/Poller.o -c /home/Mortal/MuduoProject/Muduo/Poller.cc

CMakeFiles/mymuduo.dir/Poller.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/Poller.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/Poller.cc > CMakeFiles/mymuduo.dir/Poller.i

CMakeFiles/mymuduo.dir/Poller.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/Poller.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/Poller.cc -o CMakeFiles/mymuduo.dir/Poller.s

CMakeFiles/mymuduo.dir/Poller.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/Poller.o.requires

CMakeFiles/mymuduo.dir/Poller.o.provides: CMakeFiles/mymuduo.dir/Poller.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/Poller.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/Poller.o.provides

CMakeFiles/mymuduo.dir/Poller.o.provides.build: CMakeFiles/mymuduo.dir/Poller.o

CMakeFiles/mymuduo.dir/Log.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/Log.o: /home/Mortal/MuduoProject/Muduo/Log.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/Log.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/Log.o -c /home/Mortal/MuduoProject/Muduo/Log.cc

CMakeFiles/mymuduo.dir/Log.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/Log.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/Log.cc > CMakeFiles/mymuduo.dir/Log.i

CMakeFiles/mymuduo.dir/Log.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/Log.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/Log.cc -o CMakeFiles/mymuduo.dir/Log.s

CMakeFiles/mymuduo.dir/Log.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/Log.o.requires

CMakeFiles/mymuduo.dir/Log.o.provides: CMakeFiles/mymuduo.dir/Log.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/Log.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/Log.o.provides

CMakeFiles/mymuduo.dir/Log.o.provides.build: CMakeFiles/mymuduo.dir/Log.o

CMakeFiles/mymuduo.dir/CurrentThread.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/CurrentThread.o: /home/Mortal/MuduoProject/Muduo/CurrentThread.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/CurrentThread.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/CurrentThread.o -c /home/Mortal/MuduoProject/Muduo/CurrentThread.cc

CMakeFiles/mymuduo.dir/CurrentThread.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/CurrentThread.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/CurrentThread.cc > CMakeFiles/mymuduo.dir/CurrentThread.i

CMakeFiles/mymuduo.dir/CurrentThread.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/CurrentThread.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/CurrentThread.cc -o CMakeFiles/mymuduo.dir/CurrentThread.s

CMakeFiles/mymuduo.dir/CurrentThread.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/CurrentThread.o.requires

CMakeFiles/mymuduo.dir/CurrentThread.o.provides: CMakeFiles/mymuduo.dir/CurrentThread.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/CurrentThread.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/CurrentThread.o.provides

CMakeFiles/mymuduo.dir/CurrentThread.o.provides.build: CMakeFiles/mymuduo.dir/CurrentThread.o

CMakeFiles/mymuduo.dir/Timestamp.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/Timestamp.o: /home/Mortal/MuduoProject/Muduo/Timestamp.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/Timestamp.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/Timestamp.o -c /home/Mortal/MuduoProject/Muduo/Timestamp.cc

CMakeFiles/mymuduo.dir/Timestamp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/Timestamp.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/Timestamp.cc > CMakeFiles/mymuduo.dir/Timestamp.i

CMakeFiles/mymuduo.dir/Timestamp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/Timestamp.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/Timestamp.cc -o CMakeFiles/mymuduo.dir/Timestamp.s

CMakeFiles/mymuduo.dir/Timestamp.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/Timestamp.o.requires

CMakeFiles/mymuduo.dir/Timestamp.o.provides: CMakeFiles/mymuduo.dir/Timestamp.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/Timestamp.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/Timestamp.o.provides

CMakeFiles/mymuduo.dir/Timestamp.o.provides.build: CMakeFiles/mymuduo.dir/Timestamp.o

CMakeFiles/mymuduo.dir/EpollPoller.o: CMakeFiles/mymuduo.dir/flags.make
CMakeFiles/mymuduo.dir/EpollPoller.o: /home/Mortal/MuduoProject/Muduo/EpollPoller.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/Mortal/MuduoProject/build/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mymuduo.dir/EpollPoller.o"
	/usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mymuduo.dir/EpollPoller.o -c /home/Mortal/MuduoProject/Muduo/EpollPoller.cc

CMakeFiles/mymuduo.dir/EpollPoller.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mymuduo.dir/EpollPoller.i"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/Mortal/MuduoProject/Muduo/EpollPoller.cc > CMakeFiles/mymuduo.dir/EpollPoller.i

CMakeFiles/mymuduo.dir/EpollPoller.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mymuduo.dir/EpollPoller.s"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/Mortal/MuduoProject/Muduo/EpollPoller.cc -o CMakeFiles/mymuduo.dir/EpollPoller.s

CMakeFiles/mymuduo.dir/EpollPoller.o.requires:
.PHONY : CMakeFiles/mymuduo.dir/EpollPoller.o.requires

CMakeFiles/mymuduo.dir/EpollPoller.o.provides: CMakeFiles/mymuduo.dir/EpollPoller.o.requires
	$(MAKE) -f CMakeFiles/mymuduo.dir/build.make CMakeFiles/mymuduo.dir/EpollPoller.o.provides.build
.PHONY : CMakeFiles/mymuduo.dir/EpollPoller.o.provides

CMakeFiles/mymuduo.dir/EpollPoller.o.provides.build: CMakeFiles/mymuduo.dir/EpollPoller.o

# Object files for target mymuduo
mymuduo_OBJECTS = \
"CMakeFiles/mymuduo.dir/DefaultPoll.o" \
"CMakeFiles/mymuduo.dir/EventLoop.o" \
"CMakeFiles/mymuduo.dir/Channel.o" \
"CMakeFiles/mymuduo.dir/TcpServer.o" \
"CMakeFiles/mymuduo.dir/InetAddress.o" \
"CMakeFiles/mymuduo.dir/Poller.o" \
"CMakeFiles/mymuduo.dir/Log.o" \
"CMakeFiles/mymuduo.dir/CurrentThread.o" \
"CMakeFiles/mymuduo.dir/Timestamp.o" \
"CMakeFiles/mymuduo.dir/EpollPoller.o"

# External object files for target mymuduo
mymuduo_EXTERNAL_OBJECTS =

/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/DefaultPoll.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/EventLoop.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/Channel.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/TcpServer.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/InetAddress.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/Poller.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/Log.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/CurrentThread.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/Timestamp.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/EpollPoller.o
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/build.make
/home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so: CMakeFiles/mymuduo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library /home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mymuduo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mymuduo.dir/build: /home/Mortal/MuduoProject/Muduo/dir/libmymuduo.so
.PHONY : CMakeFiles/mymuduo.dir/build

CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/DefaultPoll.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/EventLoop.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/Channel.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/TcpServer.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/InetAddress.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/Poller.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/Log.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/CurrentThread.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/Timestamp.o.requires
CMakeFiles/mymuduo.dir/requires: CMakeFiles/mymuduo.dir/EpollPoller.o.requires
.PHONY : CMakeFiles/mymuduo.dir/requires

CMakeFiles/mymuduo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mymuduo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mymuduo.dir/clean

CMakeFiles/mymuduo.dir/depend:
	cd /home/Mortal/MuduoProject/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/Mortal/MuduoProject/Muduo /home/Mortal/MuduoProject/Muduo /home/Mortal/MuduoProject/build /home/Mortal/MuduoProject/build /home/Mortal/MuduoProject/build/CMakeFiles/mymuduo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mymuduo.dir/depend

