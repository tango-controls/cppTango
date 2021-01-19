cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(cppTango)

# omniORB>=4.2.4 is required for compilation in C++17 (or better) mode.
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(BUILD_SHARED_LIBS "Build a shared library instead of static" ON)

option(USE_PCH "Use precompiled header for server/tango.h" OFF)
if (USE_PCH AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "GNU|MSVC|Clang"))
    message(WARNING "Precompiled headers are not supported for selected compiler.")
    set(USE_PCH OFF)
endif()

option(TANGO_ENABLE_COVERAGE "Instrument code for coverage analysis" OFF)
if (TANGO_ENABLE_COVERAGE AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
    message(WARNING "Code coverage is not supported for selected compiler.")
    set(TANGO_ENABLE_COVERAGE OFF)
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

  find_program(DOCKER_BINARY docker)
  if(NOT DOCKER_BINARY)
      message(WARNING "The tests can not be run as docker is missing.")
  endif()

  if(BUILD_SHARED_LIBS)
    add_subdirectory("cpp_test_suite")
  else()
    message(WARNING "Not building the tests, because that is currently supported only when BUILD_SHARED_LIBS is ON")
    set(BUILD_TESTING OFF)
  endif()
endif()

if(WIN32)
    include(configure/cmake_win.cmake)
    include(configure/cpack_win.cmake)
else()
    include(configure/cmake_linux.cmake)
endif()

include(CPack)
