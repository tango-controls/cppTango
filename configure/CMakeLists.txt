include(FindPkgConfig)
include(FindDoxygen)
include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#assuming build platform == target
set(PLATFORM 32)
if(WIN32)
    if(CMAKE_CL_64)
        set(PLATFORM 64)
    endif()
else()
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set(PLATFORM 64)
    endif()
endif()

message(STATUS "CMake: version ${CMAKE_VERSION}")
message(STATUS "Target platform: ${CMAKE_SYSTEM_NAME} ${PLATFORM}-bit")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} with version ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} with version ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Environment: TANGO_HOST=$ENV{TANGO_HOST}")

if(NOT TANGO_DEVICE_SERVER_PATH)
    #TODO windows
    set(TANGO_DEVICE_SERVER_PATH ${CMAKE_INSTALL_FULL_BINDIR})
endif()

if(CPPZMQ_BASE)
    message("Using CPPZMQ_BASE=${CPPZMQ_BASE}")
    include_directories(SYSTEM ${CPPZMQ_BASE}/include)
endif()

if(IDL_BASE)
    message("Using IDL_BASEIDL_BASE=${IDL_BASE}")
    set(IDL_PKG_INCLUDE_DIRS ${IDL_BASE}/include)
else()
    pkg_search_module(IDL_PKG REQUIRED tangoidl)
endif()

if(OMNI_BASE)
    message("Using OMNI_BASE=${OMNI_BASE}")
    set(OMNIORB_PKG_INCLUDE_DIRS ${OMNI_BASE}/include)
    if(WIN32)
        set(OMNIIDL_PATH ${OMNI_BASE}/bin/x86_win32/)
        link_directories(${OMNI_BASE}/lib/x86_win32/)
        include_directories(SYSTEM ${OMNI_BASE}/include)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(OMNIORB_PKG_LIBRARIES_DYN "omniORB4_rtd.lib;omniDynamic4_rtd.lib;omnithread_rtd.lib;COS4_rtd.lib")
            set(OMNIORB_PKG_LIBRARIES_STA "omniORB4d.lib;omniDynamic4d.lib;omnithreadd.lib;COS4d.lib")
        else()
            set(OMNIORB_PKG_LIBRARIES_DYN "omniORB4_rt.lib;omniDynamic4_rt.lib;omnithread_rt.lib;COS4_rt.lib")
            set(OMNIORB_PKG_LIBRARIES_STA "omniORB4.lib;omniDynamic4.lib;omnithread.lib;COS4.lib")
        endif()
    else()
        set(OMNIIDL_PATH ${OMNI_BASE}/bin/)
        include_directories(SYSTEM ${OMNI_BASE}/include)
        link_directories(${OMNI_BASE}/lib)
        set(OMNIORB_PKG_LIBRARIES "-lomniORB4 -lomnithread -lCOS4 -lomniDynamic4")
    endif()
else()
    #CORBA
    pkg_search_module(OMNIORB_PKG REQUIRED omniORB4)
    #COS
    pkg_search_module(OMNICOS_PKG REQUIRED omniCOS4)
    #dynamic CORBA
    pkg_search_module(OMNIDYN_PKG REQUIRED omniDynamic4)
    if(NOT OMNIORB_PKG_FOUND)
        message(FATAL_ERROR "omniORB4 library is required for Tango")
    endif()
endif()

if(ZMQ_BASE)
    message("Using ZMQ_BASE=${ZMQ_BASE}")
    set(ZMQ_PKG_INCLUDE_DIRS ${ZMQ_BASE}/include)
    if (WIN32)
        link_directories(${ZMQ_BASE}/lib/Debug)
        link_directories(${ZMQ_BASE}/lib/Release)
        link_directories(${ZMQ_BASE}/bin/Debug)
        link_directories(${ZMQ_BASE}/bin/Release)
        if(CMAKE_VS_PLATFORM_TOOLSET STREQUAL "v140")
            if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                set(ZMQ_PKG_LIBRARIES_STA "libzmq-v140-mt-sgd-4_0_5.lib")
                set(ZMQ_PKG_LIBRARIES_DYN "libzmq-v140-mt-gd-4_0_5.lib")
            else()
                set(ZMQ_PKG_LIBRARIES_STA "libzmq-v140-mt-s-4_0_5.lib")
                set(ZMQ_PKG_LIBRARIES_DYN "libzmq-v140-mt-4_0_5.lib")
            endif()
        endif()
        if(CMAKE_VS_PLATFORM_TOOLSET STREQUAL "v141")
            if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                set(ZMQ_PKG_LIBRARIES_STA "libzmq-v141-mt-sgd-4_0_5.lib")
                set(ZMQ_PKG_LIBRARIES_DYN "libzmq-v141-mt-gd-4_0_5.lib")
            else()
                set(ZMQ_PKG_LIBRARIES_STA "libzmq-v141-mt-s-4_0_5.lib")
                set(ZMQ_PKG_LIBRARIES_DYN "libzmq-v141-mt-4_0_5.lib")
            endif()
        endif()
    else()
        set(ZMQ_PKG_LIBRARIES "-lzmq")
        link_directories(${ZMQ_BASE}/lib)
    endif()
    include_directories(SYSTEM ${ZMQ_BASE}/include)
else()
    pkg_search_module(ZMQ_PKG REQUIRED libzmq)
    if(NOT ZMQ_PKG_FOUND)
        message(FATAL_ERROR "ZMQ library is required for Tango")
    endif()
endif()

if(PTHREAD_WIN)
    message("Using PTHREAD_WIN=${PTHREAD_WIN}")
    add_definitions(-DHAVE_PTHREAD_NP_H)
    set(PTHREAD_WIN_PKG_INCLUDE_DIRS ${PTHREAD_WIN}/include)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(PTHREAD_WIN_PKG_LIBRARIES_DYN "pthreadVC2d.lib")
        set(PTHREAD_WIN_PKG_LIBRARIES_STA "pthreadVC2-sd.lib")
    else()
        set(PTHREAD_WIN_PKG_LIBRARIES_DYN "pthreadVC2.lib")
        set(PTHREAD_WIN_PKG_LIBRARIES_STA "pthreadVC2-s.lib")
    endif()
endif()

#####  Ensure zmq.hpp file is present #####
include(CheckIncludeFileCXX)
# set CMAKE_REQUIRED_INCLUDES to take the currently defined include directories into account when looking for zmq.hpp
get_property(CMAKE_REQUIRED_INCLUDES DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
# Clear HAVE_ZMQ_HPP variable from the cache to force the check every time
# Thanks to this, the user do not have to clear the CMake cache himself when using different -DCPPZMQ_BASE options
unset(HAVE_ZMQ_HPP CACHE)
CHECK_INCLUDE_FILE_CXX(zmq.hpp HAVE_ZMQ_HPP)
if(NOT HAVE_ZMQ_HPP)
    message(FATAL_ERROR "zmq.hpp include file not found. \n
Please install cppzmq (https://github.com/zeromq/cppzmq) on your system and/or use \
-DCPPZMQ_BASE=<cppzmq home folder> option to specify where zmq.hpp file is installed \
(CMake will look for it under <CPPZMQ_BASE>/include, <ZMQ_BASE>/include, \
<OMNI_BASE>/include and system include directories).")
endif()

if(NOT WIN32)
  try_compile(TANGO_ZMQ_HAS_DISCONNECT ${CMAKE_BINARY_DIR}/test_cppzmq_disconnect
              SOURCES ${CMAKE_SOURCE_DIR}/configure/test_cppzmq_disconnect.cpp
              COMPILE_DEFINITIONS "-I ${CPPZMQ_BASE}/include -I ${ZMQ_BASE}/include"
              LINK_LIBRARIES "${ZMQ_PKG_LIBRARIES}")

  message(STATUS "Check if zmq::socket has a disconnect method: ${TANGO_ZMQ_HAS_DISCONNECT}")

  set(ZMQ_MIN_VER_MAJOR 4)
  set(ZMQ_MIN_VER_MINOR 0)
  set(ZMQ_MIN_VER_PATCH 5)

  try_compile(TANGO_ZMQ_USABLE_VERSION ${CMAKE_BINARY_DIR}/test_zmq_version
              SOURCES ${CMAKE_SOURCE_DIR}/configure/test_zmq_version.cpp
              COMPILE_DEFINITIONS "-I ${ZMQ_BASE}/include
              -DMINIMUM_VERSION_MAJOR=${ZMQ_MIN_VER_MAJOR}
              -DMINIMUM_VERSION_MINOR=${ZMQ_MIN_VER_MINOR}
              -DMINIMUM_VERSION_PATCH=${ZMQ_MIN_VER_PATCH}"
              LINK_LIBRARIES "${ZMQ_PKG_LIBRARIES}")

  set(msg "Check if libzmq version is >= ${ZMQ_MIN_VER_MAJOR}.${ZMQ_MIN_VER_MINOR}.${ZMQ_MIN_VER_PATCH}: ${TANGO_ZMQ_USABLE_VERSION}")

  if(${TANGO_ZMQ_USABLE_VERSION})
    message(STATUS ${msg})
  else()
    message(FATAL_ERROR ${msg})
  endif()
else()
  set(TANGO_ZMQ_HAS_DISCONNECT TRUE)
  message(STATUS "Check if zmq::socket has a disconnect method: ${TANGO_ZMQ_HAS_DISCONNECT} (hardcoded)")
endif()

if(NOT WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message("found GNU compiler ...")
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            message("setup for release build ...")
            add_compile_definitions(_FORTIFY_SOURCE=2)
            add_compile_options(-fstack-protector-strong)
        else()
            message("setup for debug build ...")
            add_compile_options(-Wall -Wextra -Wformat -Werror=format-security -pedantic -Og -g)
        endif()
    endif()
endif()

if (TANGO_ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

if(WARNINGS_AS_ERRORS)
  if(WIN32)
    add_compile_options(/WX)
  else()
    add_compile_options(-Werror)
  endif()
endif()

include(GNUInstallDirs)
include(configure/coveralls.cmake)

option(TANGO_USE_USING_NAMESPACE "Use \"using namespace\" in header files (deprecated, but ON for backwards compatibility.)." ON)
option(TANGO_JPEG_MMX "Build MMX support" ON)
