project(test_suite)

set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g")

add_compile_options(-std=c++1y)

configure_file(CTestCustom.cmake CTestCustom.cmake)

#TODO different OS
set(SERV_NAME "DevTest")
set(INST_NAME "test")
set(DEV1 "${INST_NAME}/debian8/10")
set(DEV2 "${INST_NAME}/debian8/11")
set(DEV3 "${INST_NAME}/debian8/12")
set(DEV20 "${INST_NAME}2/debian8/20")
set(FWD_DEV "${INST_NAME}/fwd_debian8/10")
set(DEV1_ALIAS "debian8_alias")
set(ATTR_ALIAS "debian8_attr_alias")
cmake_host_system_information(RESULT HOST_NAME QUERY HOSTNAME)
string(TOLOWER ${HOST_NAME} HOST_NAME)
message("HOST_NAME=${HOST_NAME}")

add_subdirectory(environment)

macro(TEST_SUITE_ADD_TEST test)
    message("Add executable ${test}")
    add_executable(${test} ${test}.cpp)
    target_compile_definitions(${test} PUBLIC "-DVALGRIND -D_PTHREADS -D_REENTRANT")
    target_link_libraries(${test} tango)
        #TODO generalize tests
#    add_test(NAME "CPP::${test}"  COMMAND $<TARGET_FILE:${test}> ${DEV1} ${DEV2} ${DEV3} ${DEV1_ALIAS})
endmacro()

add_subdirectory(cxxtest)

add_subdirectory(old_tests)
add_subdirectory(new_tests)


add_subdirectory(asyn)
add_subdirectory(event)

add_subdirectory(cpp_test_ds)