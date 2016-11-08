macro(ADD_CGREEN_TEST test)
    add_executable(${test} ${test}.cxx)
    target_link_libraries(${test} PRIVATE tango ${CGREEN_LIBRARIES})
    target_include_directories(${test} PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${CGREEN_INCLUDE_DIRS})
    target_compile_options(${test} PRIVATE -std=c++1y)

    add_test(NAME "cgreen::${test}" COMMAND $<TARGET_FILE:${test}>)
endmacro()




add_cgreen_test(test_pollthread)