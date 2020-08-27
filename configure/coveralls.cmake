option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    # Add project cmake modules to path.
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${COVERALLS_MODULE_PATH})

    include(Coveralls)
    coveralls_turn_on_coverage()

    FILE(GLOB_RECURSE COVERAGE_SRCS ${PROJECT_SOURCE_DIR}/cppapi/*.tpp ${PROJECT_SOURCE_DIR}/cppapi/*.cpp ${PROJECT_SOURCE_DIR}/cppapi/*.h)

    # Create the coveralls target.
    coveralls_setup(
            "${COVERAGE_SRCS}" # The source files.
            ON                 # If we should upload.
    )
endif ()

