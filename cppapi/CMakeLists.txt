link_directories(${ZMQ_PKG_LIBRARY_DIRS} ${OMNIORB_PKG_LIBRARY_DIRS})
include_directories(${ZMQ_PKG_INCLUDE_DIRS} ${OMNIORB_PKG_INCLUDE_DIRS})

add_subdirectory("client")
add_subdirectory("server")


#link_libraries()

#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tango.pc"
#        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")