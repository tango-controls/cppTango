if(DOXYGEN_FOUND)
    configure_file(Doxyfile.cmake Doxyfile @ONLY)
    add_custom_target(docs ${DOXYGEN_EXECUTABLE} Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_target(clean-docs rm -rf ${PROJECT_SOURCE_DIR}/docs)
    #TODO install
endif()