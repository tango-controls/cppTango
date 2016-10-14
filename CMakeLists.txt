cmake_minimum_required(VERSION 2.8)
project(tango)
include(CTest)

enable_testing()
#need to define the version of the library
set(MAJOR_VERSION "9")
set(MINOR_VERSION "2")
set(PATCH_VERSION "5")

#convenient versions
set(LIBRARY_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(SO_VERSION "${MAJOR_VERSION}")


message("System TANGO_HOST=$ENV{TANGO_HOST}")
set(TANGO_HOST $ENV{TANGO_HOST})
include(configure/CMakeLists.txt)

include_directories(generated)
include_directories(include)
#required for generated config.h
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(include)
add_subdirectory(doxygen)
#source code
add_subdirectory(generated/idl)
add_subdirectory(src)
add_subdirectory(test)

add_library(tango SHARED    $<TARGET_OBJECTS:log4tango_objects>
                            $<TARGET_OBJECTS:client_objects>
                            $<TARGET_OBJECTS:idl_objects>
                            $<TARGET_OBJECTS:jpeg_objects>
                            $<TARGET_OBJECTS:jpeg_mmx_objects>
                            $<TARGET_OBJECTS:server_objects>
                            $<TARGET_OBJECTS:include_objects>)
target_include_directories(tango PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})
target_link_libraries(tango PUBLIC ${ZMQ_PKG_LIBRARIES} ${OMNIORB_PKG_LIBRARIES} ${OMNICOS_PKG_LIBRARIES} ${OMNIDYN_PKG_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})
target_compile_options(tango PRIVATE -fPIC)
target_compile_definitions(tango PRIVATE OMNI_UNLOADABLE_STUBS _TANGO_LIB)

set_target_properties(tango PROPERTIES
        VERSION ${LIBRARY_VERSION}
        SOVERSION ${SO_VERSION})

#install code
install(TARGETS tango LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")

configure_file(tango.pc.cmake tango.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")

#TODO windows
SET(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_FILE_NAME libtango_${LIBRARY_VERSION}_amd64) #TODO platform
set(CPACK_DEBIAN_PACKAGE_NAME libtango-dev)
set(CPACK_DEBIAN_PACKAGE_VERSION ${LIBRARY_VERSION})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcos4-dev, libzmq5-dev | libzmq3-dev")
set(CPACK_DEBIAN_PACKAGE_CONFLICTS "libtango7-dev, libtango8-dev, libtango9-dev")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "TANGO team <tango@esrf.fr>") #required

include(CPack)
