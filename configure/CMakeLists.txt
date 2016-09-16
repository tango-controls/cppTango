include(FindPkgConfig)

if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified - default is DEBUG")
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

if(OMNI_BASE)
    message("Using OMNI_BASE=${OMNI_BASE}")
    set(OMNIORB_PKG_INCLUDE_DIRS ${OMNI_BASE}/include)
    set(OMNIORB_PKG_LIBRARIES "-lomniORB4 -lomnithread -lCOS4 -lomniDynamic4")
    include_directories(${OMNI_BASE}/include)
    link_directories(${OMNI_BASE}/lib)
else()
    #CORBA
    pkg_search_module(OMNIORB_PKG REQUIRED omniORB4)
    #COS
    pkg_search_module(OMNICOS_PKG REQUIRED omniCOS4)
    #dynamic CORBA
    pkg_search_module(OMNIDYN_PKG REQUIRED omniDynamic4)
    if(NOT OMNIORB_PKG_FOUND)
        message(FATAL "omniORB4 library is required for Tango")
    endif()
endif()

if(ZMQ_BASE)
    message("Using ZMQ_BASE=${ZMQ_BASE}")
    set(ZMQ_PKG_INCLUDE_DIRS ${ZMQ_BASE}/include)
    set(ZMQ_PKG_LIBRARIES "-lzmq")
    link_directories(${ZMQ_BASE}/lib)
else()
    pkg_search_module(ZMQ_PKG REQUIRED libzmq)
    if(NOT ZMQ_PKG_FOUND)
        message(FATAL "ZMQ library is required for Tango")
    endif()
endif()

#setup for GNU CXX compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("found GNU compiler ...")
    if(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message("setup for release build ...")
        add_definitions(-O2 -D_TANGO_LIB)
    else()
        message("setup for debug build ...")
        add_definitions(-g -O0 -Wall -Wextra)# -pedantic
    endif()
    add_definitions(-std=c++11)
    add_definitions(-DGNU_CXX -DOMNI_UNLOADABLE_STUBS -D_FORTIFY_SOURCE=2)
    add_definitions(-fPIC)
endif()

include(GNUInstallDirs)