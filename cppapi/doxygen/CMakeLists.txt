add_custom_target(cppapi_doc doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../server
                    SOURCES Doxyfile)