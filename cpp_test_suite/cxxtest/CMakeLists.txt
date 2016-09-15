message("Generate runner.cpp")
execute_process(COMMAND python cxxtestgen.py --template=${CMAKE_CURRENT_SOURCE_DIR}/template/tango_template.tpl
        -o ${CMAKE_CURRENT_SOURCE_DIR}/runner.cpp
        ../../new_tests/cxx_syntax.cpp
        ../../new_tests/cxx_poll.cpp
        ../../new_tests/cxx_poll_admin.cpp
        ../../new_tests/cxx_dserver_cmd.cpp
        ../../new_tests/cxx_dserver_misc.cpp
        ../../new_tests/cxx_always_hook.cpp
        ../../new_tests/cxx_blackbox.cpp
        ../../new_tests/cxx_attr_misc.cpp
        ../../new_tests/cxx_attrprop.cpp
        ../../new_tests/cxx_attr_write.cpp
        ../../new_tests/cxx_attr_conf.cpp
        ../../new_tests/cxx_attr.cpp
        ../../new_tests/cxx_misc_util.cpp
        ../../new_tests/cxx_templ_cmd.cpp
        ../../new_tests/cxx_exception.cpp
        ../../new_tests/cxx_cmd_query.cpp
        ../../new_tests/cxx_seq_vec.cpp
        ../../new_tests/cxx_test_state_on.cpp
        ../../new_tests/cxx_misc.cpp
        ../../new_tests/cxx_cmd_types.cpp
        ../../new_tests/cxx_group.cpp
        ../../new_tests/cxx_encoded.cpp
        ../../new_tests/cxx_database.cpp
        ../../new_tests/cxx_mem_attr.cpp
        ../../new_tests/cxx_enum_att.cpp
        ../../new_tests/cxx_write_attr_hard.cpp
        ../../new_tests/cxx_fwd_att.cpp
        ../../new_tests/cxx_pipe_conf.cpp
        ../../new_tests/cxx_pipe.cpp
        ../../new_tests/cxx_z00_dyn_cmd.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(runner $<TARGET_OBJECTS:compare_test_object> ${CMAKE_CURRENT_SOURCE_DIR}/runner.cpp)
target_include_directories(runner PUBLIC include)
target_link_libraries(runner PUBLIC ${CMAKE_DL_LIBS} tango)

#TODO different OS
set(DEV1 "test/debian8/10")
set(DEV2 "test/debian8/11")
set(DEV3 "test/debian8/12")
set(FWD_DEV "test/fwd_debian8/10")
set(SERV_NAME "devTest")
set(INST_NAME "debian8")
set(DEV1_ALIAS "debian8_alias")
set(ATTR_ALIAS "debian8_attr_alias")
cmake_host_system_information(RESULT HOST_NAME QUERY HOSTNAME)
set(FULL_HOSTNAME "${HOST_NAME}:10000")

add_test("CXX::runner" runner
                        --device1=${DEV1}
                        --device2=${DEV2}
                        --device3=${DEV3}
                        --fwd_device=${FWD_DEV}
                        --loop=1
                        --fulldsname=${SERV_NAME}/${INST_NAME}
                        --serverhost=${FULL_HOSTNAME}
                        --clienthost=${FULL_HOSTNAME}
                        --serverversion=5
                        --docurl=http://www.tango-controls.org
                        --devtype=TestDevice
                        --dbserver=sys/database/2
                        --outpath=/tmp/
                        --refpath=${CMAKE_CURRENT_SOURCE_DIR}/../new_tests/out/
                        --loglevel=0
                        --dsloglevel=3
                        --suiteloop=1
                        --devicealias=${DEV1_ALIAS}
                        --attributealias=${ATTR_ALIAS}
)