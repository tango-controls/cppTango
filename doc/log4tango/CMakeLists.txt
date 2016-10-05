if(DOXYGEN_FOUND)
    configure_file(Doxyfile.cmake Doxyfile @ONLY)
    add_custom_target(log4tango_doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
endif()