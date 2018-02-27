project(libtango)



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