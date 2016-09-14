include(FindPkgConfig)

#cNp from Make.rules
if(NOT TANGO_HOME)
    set(TANGO_HOME /segfs/tango)
    message("Using TANGO_HOME=${TANGO_HOME}")
endif()

if(NOT ubuntu)
    execute_process(COMMAND /csadmin/common/scripts/get_os.share
            OUTPUT_VARIABLE OS)
endif()

if(NOT OS)
    if(dev)
        set(OS ubuntu1404)
    else()
        set(OS ubuntu1204)
    endif()
endif()

if(NOT OS)
    set(BIN_DIR ${OS})
    message("Using BIN_DIR=${BIN_DIR}")
endif()

if(NOT BIN_DIR)
    set(BIN_DIR ${OS})
    message("Using BIN_DIR=${BIN_DIR}")
endif()

if(NOT OMNI_BASE)
    set(OMNI_BASE ${TANGO_HOME}/ORB/omniORB4.2.1/bin)
    message("Using OMNI_BASE=${OMNI_BASE}")
endif()

if(NOT ZMQ_BASE)
    set(ZMQ_BASE ${TANGO_HOME}/transport/zmq4.0.5/bin)
    message("Using ZMQ_BASE=${ZMQ_BASE}")
endif()

#setup for GNU CXX compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("found GNU compiler ...")
    add_definitions(-g -Wall)#-Wextra -pedantic
    if(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message("setup for release build ...")
        add_definitions(-O3)
    elseif(CMAKE_BUILD_TYPE MATCHES DEBUG)
        message("setup for debug build ...")
        add_definitions(-O0)
        add_definitions(-DDEBUG)
    endif()

    add_definitions(-std=c++11)
    add_definitions(-DGNU_CXX)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("found clang compiler...")

    add_definitions(-Wextra -pedantic -Wall)
    if(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message("setup for release build ...")
        add_definitions(-O3 -g)
    elseif(CMAKE_BUILD_TYPE MATCHES DEBUG)
        message("setup for debug build ...")
        add_definitions(-O0 -g -fno-inline -DDEBUG)
    endif()

    #for this version the compiler has to support c++11 flag
    add_definitions(-std=c++11)
    add_definitions(-DCLANG_CXX)
endif()

#find libraries
#ZMQ
pkg_search_module(ZMQ_PKG REQUIRED libzmq)
if(NOT ZMQ_PKG_FOUND)
    set(ZMQ_PKG_INCLUDE_DIRS ${ZMQ_BASE}/include)
endif()

#CORBA
pkg_search_module(OMNIORB_PKG REQUIRED omniORB4)
if(NOT OMNIORB_PKG_FOUND)
    set(OMNIORB_PKG_INCLUDE_DIRS ${OMNI_BASE}/include)
endif()


