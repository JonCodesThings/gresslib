cmake_minimum_required(VERSION 3.10)
project(gresslib C)

set(CMAKE_C_STANDARD 11)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(SOURCE_FILES
		include/gresslib/gresslib.h
		src/internal/gresslib_internal.c
		src/WGL/glbootstrap.c
		src/win32/gresslib.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(SOURCE_FILES
		include/gresslib/gresslib.h
		src/internal/gresslib_internal.c
		src/glx/glbootstrap.c
		src/x11/gresslib.c)
endif()

set(EXAMPLE_SOURCE_FILES
	sandbox/basic.c)

include_directories(.)

add_library(gresslib STATIC ${SOURCE_FILES})
add_executable(gresslib_basic_example ${EXAMPLE_SOURCE_FILES})

set_target_properties(gresslib PROPERTIES PREFIX "")
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(gresslib "OpenGL32.lib")
endif()

target_link_libraries(gresslib_basic_example gresslib)
