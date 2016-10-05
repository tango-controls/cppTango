add_subdirectory(log4tango)
add_subdirectory(cppapi)

if(DOXYGEN_FOUND)
    add_custom_target(doc)
    add_dependencies(doc log4tango_doc cppapi_doc)
endif()