cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(cppTango)

set(CXX_STANDARD_REQUIRED 11)

include(CTest)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

option(USE_PCH "Use precompiled header for server/tango.h" OFF)
if (USE_PCH AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "GNU|MSVC|Clang"))
    message(WARNING "Precompiled headers are not supported for selected compiler.")
    set(USE_PCH OFF)
endif()

#need to define the version of the library
set(MAJOR_VERSION "9")
set(MINOR_VERSION "4")
set(PATCH_VERSION "0")

if(WIN32)
    include(configure/cmake_win_defs.cmake)
    include(configure/cmake_pch_win.cmake)
else()
    include(configure/cmake_pch_linux.cmake)
endif()

#convenient versions
set(LIBRARY_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(SO_VERSION "${MAJOR_VERSION}${MINOR_VERSION}")

set(TANGO_HOST $ENV{TANGO_HOST})
include(configure/CMakeLists.txt)

include_directories(cppapi/client)
include_directories(cppapi/client/helpers)
include_directories(cppapi/server)
include_directories(cppapi/server/idl)
include_directories(log4tango/include)
#required for generated config.h
include_directories(${CMAKE_CURRENT_BINARY_DIR}/cppapi/server)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/log4tango/include)

#source code
add_subdirectory("log4tango")
add_subdirectory("cppapi")

if(BUILD_TESTING)
    add_subdirectory("cpp_test_suite")
endif()

if(WIN32)
    include(configure/cmake_win.cmake)
    include(configure/cpack_win.cmake)
else(WIN32)
    include(configure/cmake_linux.cmake)
endif(WIN32)

include(CPack)
