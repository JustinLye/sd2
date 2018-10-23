cmake_minimum_required(VERSION 3.9)

option(DefineDebug "DefineDebug" OFF)
option(Trace "Trace" OFF)

if(DefineDebug)
	message(STATUS "Turning on debug mode...")
	add_definitions(-DDEBUG_)
endif(DefineDebug)

if(Trace)
	message(STATUS "Turning on trace mode...")
	add_definitions(-DTRACE_)
endif(Trace)

include_directories("include")
include_directories("external")

add_subdirectory(src)