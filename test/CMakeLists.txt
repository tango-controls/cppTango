add_subdirectory(log4tango)
add_subdirectory(cpp_test_suite)

if(CGREEN_FOUND)
    add_subdirectory(server)
else()
    message(WARNING "cgreen was not found! Skipping unit tests...")
endif()

