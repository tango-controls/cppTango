include(FindPkgConfig)

if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified - default is DEBUG")
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

if(IDL_BASE)
    set(IDL_PKG_INCLUDE_DIRS ${IDL_BASE}/include)
else(IDL_BASE)
    pkg_search_module(IDL_PKG REQUIRED tangoidl)
endif(IDL_BASE)

if(OMNI_BASE)
    message("Using OMNI_BASE=${OMNI_BASE}")
    set(OMNIORB_PKG_INCLUDE_DIRS ${OMNI_BASE}/include)
    set(OMNIORB_PKG_LIBRARIES "-lomniORB4 -lomnithread -lCOS4 -lomniDynamic4")
    set(OMNIIDL_PATH ${OMNI_BASE}/bin/)
    include_directories(${OMNI_BASE}/include)
    link_directories(${OMNI_BASE}/lib)
else(OMNI_BASE)
    #CORBA
    pkg_search_module(OMNIORB_PKG REQUIRED omniORB4)
    #COS
    pkg_search_module(OMNICOS_PKG REQUIRED omniCOS4)
    #dynamic CORBA
    pkg_search_module(OMNIDYN_PKG REQUIRED omniDynamic4)
    if(NOT OMNIORB_PKG_FOUND)
        message(FATAL "omniORB4 library is required for Tango")
    endif()
endif(OMNI_BASE)

if(ZMQ_BASE)
    message("Using ZMQ_BASE=${ZMQ_BASE}")
    set(ZMQ_PKG_INCLUDE_DIRS ${ZMQ_BASE}/include)
    set(ZMQ_PKG_LIBRARIES "-lzmq")
    include_directories(${ZMQ_BASE}/include)
    link_directories(${ZMQ_BASE}/lib)
else(ZMQ_BASE)
    pkg_search_module(ZMQ_PKG REQUIRED libzmq)
    if(NOT ZMQ_PKG_FOUND)
        message(FATAL "ZMQ library is required for Tango")
    endif()
endif(ZMQ_BASE)

message("Verifying ${OMNIIDL_PATH}omniidl")
execute_process(COMMAND ${OMNIIDL_PATH}omniidl -V RESULT_VARIABLE FAILED)

if(${FAILED})
    message(SEND_ERROR " No omniidl was found! rv=${FAILED}")
endif()

#assuming build platform == target
set(PLATFORM 32)
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(PLATFORM 64)
endif()

#setup for GNU CXX compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("found GNU compiler ...")
    if(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message("setup for release build ...")
        add_definitions(-D_FORTIFY_SOURCE=3)
    else()
        message("setup for debug build ...")
        add_compile_options(-O0 -Wall -Wextra)# -pedantic
    endif()
    add_compile_options(-std=c++11 -fPIC)
    add_definitions(-DGNU_CXX)
endif()

include(GNUInstallDirs)
