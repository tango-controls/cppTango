#TODO windows
message("System TANGO_HOST=$ENV{TANGO_HOST}")
set(TANGO_HOST $ENV{TANGO_HOST})
configure_file(tango_host.cmake tango_host @ONLY)

add_custom_target(start-tango /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/setup.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        SOURCES setup.sh)

add_custom_target(stop-tango /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/shutdown.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        SOURCES shutdown.sh)