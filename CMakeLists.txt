cmake_minimum_required(VERSION 2.8.12)
include(CTest)

enable_testing()
#need to define the version of the library
set(MAJOR_VERSION "9")
set(MINOR_VERSION "3")
set(PATCH_VERSION "6")

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
if(NOT WIN32)
    add_subdirectory("cpp_test_suite")
endif(NOT WIN32)

if(WIN32)
    include(configure/cmake_win.cmake)
else(WIN32)
    add_library(tango SHARED    $<TARGET_OBJECTS:log4tango_objects>
                                $<TARGET_OBJECTS:client_objects>
                                $<TARGET_OBJECTS:idl_objects>
                                $<TARGET_OBJECTS:jpeg_objects>
                                $<TARGET_OBJECTS:jpeg_mmx_objects>
                                $<TARGET_OBJECTS:server_objects>)
    target_link_libraries(tango PUBLIC ${ZMQ_PKG_LIBRARIES} ${OMNIORB_PKG_LIBRARIES} ${OMNICOS_PKG_LIBRARIES} ${OMNIDYN_PKG_LIBRARIES} ${CMAKE_DL_LIBS})
    target_compile_options(tango PRIVATE -fPIC)
    target_include_directories(tango PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})

    target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
    target_compile_definitions(tango PRIVATE OMNI_UNLOADABLE_STUBS _TANGO_LIB)

    set_target_properties(tango PROPERTIES
                          VERSION ${LIBRARY_VERSION}
                          SOVERSION ${SO_VERSION})
        
    install(TARGETS tango LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
    configure_file(tango.pc.cmake tango.pc @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
            DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")
endif(WIN32)


if(WIN32)
    include(configure/cpack_win.cmake)
else(WIN32)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_NAME libtango-dev)
    set(CPACK_DEBIAN_PACKAGE_VERSION ${LIBRARY_VERSION})
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcos4-dev, libzmq5-dev | libzmq3-dev")
    set(CPACK_DEBIAN_PACKAGE_CONFLICTS "libtango7-dev, libtango8-dev, libtango9-dev")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "TANGO team <tango@esrf.fr>") #required
endif(WIN32)
include(CPack)
