cmake_minimum_required(VERSION 3.10)
project(gresslib C)

set(CMAKE_C_STANDARD 11)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(SOURCE_FILES
		include/gresslib/gresslib.h
		src/internal/gresslib_internal.c
		src/win32/gresslib.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(SOURCE_FILES
		include/gresslib/gresslib.h
		src/internal/gresslib_internal.c
		src/glx/glbootstrap.c
		src/x11/gresslib.c)
endif()

include_directories(.)

add_library(gresslib STATIC ${SOURCE_FILES})
set_target_properties(gresslib PROPERTIES PREFIX "")
