include(${CMAKE_CURRENT_SOURCE_DIR}/config/config.cmake)

add_subdirectory(include)
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
