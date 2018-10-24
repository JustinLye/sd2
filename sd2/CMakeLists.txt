cmake_minimum_required(VERSION 3.9)

option(Debug "Debug" OFF)
option(Trace "Trace" OFF)

set(CMAKE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(Trace)
	message(STATUS "Turning on trace mode...")
	add_definitions(-DTRACE_)
	message(STATUS "Turning on debug mode...")
	add_definitions(-DDEBUG_)
elseif(Debug)
	message(STATUS "Turning on debug mode...")
	add_definitions(-DDEBUG_)
endif(Trace)

include_directories("include")
include_directories("external")

add_subdirectory(src)
