project(test_suite)

set(CMAKE_CXX_FLAGS_RELEASE "-O0 -g")

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
set(COMMON_TEST_CXX_DEFS -DVALGRIND -D_PTHREADS -D_REENTRANT)
cmake_host_system_information(RESULT HOST_NAME QUERY FQDN)
string(TOLOWER ${HOST_NAME} HOST_NAME)
message("HOST_NAME=${HOST_NAME}")

add_subdirectory(environment)

function(TEST_SUITE_ADD_TEST test)
    message("Add executable ${test}")
    add_executable(${test} ${test}.cpp)
    target_compile_definitions(${test} PUBLIC VALGRIND _PTHREADS)
    target_link_libraries(${test} tango ${CMAKE_DL_LIBS})
    if(USE_PCH)
        tango_target_test_pch(${test} tango_test_pch)
    endif()
        #TODO generalize tests
#    add_test(NAME "CPP::${test}"  COMMAND $<TARGET_FILE:${test}> ${DEV1} ${DEV2} ${DEV3} ${DEV1_ALIAS})
endfunction()

function(tango_add_test name_tag name command_tag command)
    add_test(
        NAME "${name}"
        COMMAND "${CMAKE_BINARY_DIR}/cpp_test_suite/environment/run_with_fixture.sh" "${command}"
        ${ARGN})
endfunction()

add_subdirectory(cxxtest)

add_subdirectory(old_tests)
add_subdirectory(new_tests)


add_subdirectory(asyn)
add_subdirectory(event)

add_subdirectory(cpp_test_ds)

if(USE_PCH)
    tango_add_pch(tango_test_pch cxx_syntax "${OMNIORB_PKG_CFLAGS_OTHER}")
endif()
