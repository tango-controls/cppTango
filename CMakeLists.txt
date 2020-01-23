cmake_minimum_required(VERSION 2.8.12)
include(CTest)

#need to define the version of the library
set(MAJOR_VERSION "9")
set(MINOR_VERSION "3")
set(PATCH_VERSION "4")

if(WIN32)
    include(configure/cmake_win_defs.cmake)
endif(WIN32)

#convenient versions
set(LIBRARY_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(SO_VERSION "${MAJOR_VERSION}")

message("System TANGO_HOST=$ENV{TANGO_HOST}")
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
