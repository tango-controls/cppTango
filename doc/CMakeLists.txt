add_subdirectory(log4tango)

if(DOXYGEN_FOUND)
    add_custom_target(doc)
    add_dependencies(doc log4tango_doc)
endif()