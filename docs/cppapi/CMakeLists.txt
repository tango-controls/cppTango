if(DOXYGEN_FOUND)
    configure_file(Doxyfile.cmake Doxyfile @ONLY)
    add_custom_target(cppapi_doc ${DOXYGEN_EXECUTABLE} Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()