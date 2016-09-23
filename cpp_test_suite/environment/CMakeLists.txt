#TODO windows
add_custom_target(start-tango /bin/bash setup.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCES setup.sh)

add_custom_target(stop-tango /bin/bash shutdown.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCES shutdown.sh)