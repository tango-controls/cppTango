project(libtango)

add_library(tango $<TARGET_OBJECTS:log4tango_objects>
                  $<TARGET_OBJECTS:client_objects>
                  $<TARGET_OBJECTS:idl_objects>
                  $<TARGET_OBJECTS:jpeg_objects>
                  $<TARGET_OBJECTS:jpeg_mmx_objects>
                  $<TARGET_OBJECTS:server_objects>)
target_link_libraries(tango PUBLIC ${ZMQ_PKG_LIBRARIES} ${OMNIORB_PKG_LIBRARIES} ${OMNICOS_PKG_LIBRARIES} ${OMNIDYN_PKG_LIBRARIES} ${CMAKE_DL_LIBS})
target_link_libraries(tango PRIVATE $<$<BOOL:${TANGO_USE_LIBCPP}>:c++abi> $<$<BOOL:${TANGO_USE_LIBCPP}>:c++>)
target_include_directories(tango SYSTEM PUBLIC ${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS} ${OMNIDYN_PKG_INCLUDE_DIRS})
target_compile_options(tango PUBLIC ${ZMQ_PKG_CFLAGS_OTHER} ${OMNIORB_PKG_CFLAGS_OTHER} ${OMNICOS_PKG_CFLAGS_OTHER} ${OMNIDYN_PKG_CFLAGS_OTHER})

if(BUILD_SHARED_LIBS)
  target_compile_options(tango PRIVATE -fPIC)
  target_compile_definitions(tango PUBLIC _REENTRANT)
  set_target_properties(tango PROPERTIES
                        VERSION ${LIBRARY_VERSION}
                        SOVERSION ${SO_VERSION})
  install(TARGETS tango LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
else()
  install(TARGETS tango ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")
endif()

configure_file(tango.pc.cmake tango.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")

#CPack
include(configure/cpack_linux.cmake)

find_program(LSB_RELEASE lsb_release)
if(NOT LSB_RELEASE)
    message(STATUS "Can not find lsb_release in your path, default to jessie.")
    set(LINUX_FLAVOUR jessie)
endif()
execute_process(COMMAND ${LSB_RELEASE} -cs
        OUTPUT_VARIABLE LINUX_FLAVOUR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

find_program(CURL curl)
if(CURL)
    add_custom_target(deploy
            COMMAND ${CURL} -v -T ${CPACK_PACKAGE_FILE_NAME}.deb -u$ENV{BINTRAY_USER_NAME}:$ENV{BINTRAY_API_KEY} \"https://api.bintray.com/content/tango-controls/debian/cppTango/${LIBRARY_VERSION}/pool/main/libt/${LINUX_FLAVOUR}/${CPACK_PACKAGE_FILE_NAME}.deb\;deb_distribution=${LINUX_FLAVOUR}\;deb_component=main\;deb_architecture=${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}\;publish=1\;override=1\"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS ${PROJECT_BINARY_DIR}/${CPACK_PACKAGE_FILE_NAME}.deb)
else()
    message(WARNING "curl was not found deploy won't be possible")
endif()
