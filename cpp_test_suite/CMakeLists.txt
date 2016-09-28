#TODO different OS
set(DEV1 "test/debian8/10")
set(DEV2 "test/debian8/11")
set(DEV3 "test/debian8/12")
set(FWD_DEV "test/fwd_debian8/10")
set(SERV_NAME "DevTest")
set(INST_NAME "debian8")
set(DEV1_ALIAS "debian8_alias")
set(ATTR_ALIAS "debian8_attr_alias")
cmake_host_system_information(RESULT HOST_NAME QUERY HOSTNAME)
set(FULL_HOSTNAME "${HOST_NAME}:10000")

add_subdirectory(environment)

macro(TEST_SUITE_ADD_TEST test)
    message("Add test ${test}")
    add_executable(${test} ${test}.cpp)
    target_compile_definitions(${test} PUBLIC "-DVALGRIND -D_PTHREADS -D_REENTRANT")
#    target_include_directories(${test} PUBLIC ../cxxtest/include)
    target_link_libraries(${test} tango ${CMAKE_DL_LIBS})
#    add_test(NAME "CPP::${test}"  COMMAND $<TARGET_FILE:${test}> ${DEV1} ${DEV2} ${DEV3} ${DEV1_ALIAS})
endmacro()

add_subdirectory(cxxtest)

add_subdirectory(old_tests)
add_subdirectory(new_tests)


add_subdirectory(asyn)
add_subdirectory(event)