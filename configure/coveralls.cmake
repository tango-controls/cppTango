option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    # Add project cmake modules to path.
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${COVERALLS_MODULE_PATH})

    include(Coveralls)
    coveralls_turn_on_coverage()

    FILE(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

    # Create the coveralls target.
    coveralls_setup(
            "${SOURCES}" # The source files.
            ON                 # If we should upload.
    )
endif ()

