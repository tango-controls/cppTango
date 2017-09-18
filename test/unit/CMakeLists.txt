enable_testing()
#find_package(GTest REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)

macro(ADD_UNIT_TEST test args)
    message("Add executable ${test}")
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} PRIVATE tango)
    add_test(NAME unit::${test} COMMAND $<TARGET_FILE:${test}> "${args}")
endmacro()

ADD_UNIT_TEST(tango_common_utils_test "")
