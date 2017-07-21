option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    execute_process(COMMAND git clone https://github.com/JoakimSoderberg/coveralls-cmake ${PROJECT_BINARY_DIR}/coveralls-cmake)

    # Add project cmake modules to path.
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_BINARY_DIR}/coveralls-cmake/cmake)

    include(Coveralls)
    coveralls_turn_on_coverage()

    FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

    # Create the coveralls target.
    coveralls_setup(
            "${SOURCES}" # The source files.
            ON                 # If we should upload.
    )
endif ()

