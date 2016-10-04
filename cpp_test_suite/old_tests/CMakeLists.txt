set(TESTS   acc_right
            add_rem_attr
            add_rem_dev
            allowed_cmd
            att_conf
            attr_conf_test
            attr_manip
            attr_misc
            attr_proxy
            attr_types
            cmd_inout
            cmd_types
            ConfEventBugClient
            copy_devproxy
            ds_cache
            helper
            lock
            locked_device
            mem_att
            misc_devattr
            misc_devdata
            misc_devproxy
            multiple_new
            new_devproxy
            obj_prop
            poll_attr
            Poll
            poll_except
            poll_met
            poll_pool
            print_data
            print_data_hist
            prop_list
            rds
            read_attr
            read_hist
            read_hist_ext
            reconnect_attr
            reconnect
            restart_device
            ring_depth
            size
            state_attr
            StopPoll
            sub_dev
            unlock
            wait_mcast_dev
            w_r_attr
            write_attr_3
            write_attr)

foreach(TEST ${TESTS})
    TEST_SUITE_ADD_TEST(${TEST})
endforeach(TEST)


configure_file(locked_device_cmd.h.cmake locked_device_cmd.h @ONLY)
target_include_directories(lock PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME "old_tests::misc_devproxy"  COMMAND $<TARGET_FILE:misc_devproxy> ${DEV1} ${SERV_NAME}/${INST_NAME})

add_test(NAME "old_tests::cmd_types"  COMMAND $<TARGET_FILE:cmd_types> ${DEV1} 10)

add_test(NAME "old_tests::attr_types"  COMMAND $<TARGET_FILE:attr_types> ${DEV1} 10)
add_test(NAME "old_tests::att_conf"  COMMAND $<TARGET_FILE:att_conf> ${DEV1})
add_test(NAME "old_tests::misc_devdata"  COMMAND $<TARGET_FILE:misc_devdata>)
add_test(NAME "old_tests::misc_devattr"  COMMAND $<TARGET_FILE:misc_devattr>)
add_test(NAME "old_tests::write_attr"  COMMAND $<TARGET_FILE:write_attr> ${DEV1} 10)
add_test(NAME "old_tests::copy_devproxy"  COMMAND $<TARGET_FILE:copy_devproxy> ${DEV1} ${DEV2} ${DEV3})
add_test(NAME "old_tests::attr_misc"  COMMAND $<TARGET_FILE:attr_misc> ${DEV1})
add_test(NAME "old_tests::obj_prop"  COMMAND $<TARGET_FILE:obj_prop>)

configure_file(test_polling.sh.cmake test_polling.sh @ONLY)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/test_polling.sh)
add_test(NAME "old_tests::polling" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_polling.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME "old_tests::attr_proxy"  COMMAND $<TARGET_FILE:attr_proxy> ${DEV1}/Short_attr_rw)
add_test(NAME "old_tests::write_attr_3"  COMMAND $<TARGET_FILE:write_attr_3> ${DEV1} 10)
add_test(NAME "old_tests::read_hist_ext"  COMMAND $<TARGET_FILE:read_hist_ext> ${DEV1})
add_test(NAME "old_tests::ring_depth"  COMMAND $<TARGET_FILE:ring_depth> ${DEV1})
add_test(NAME "old_tests::mem_att"  COMMAND $<TARGET_FILE:mem_att> ${DEV1})
add_test(NAME "old_tests::state_attr"  COMMAND $<TARGET_FILE:state_attr> ${DEV1})
add_test(NAME "old_tests::rds"  COMMAND $<TARGET_FILE:rds> ${DEV1})
add_test(NAME "old_tests::ds_cache"  COMMAND $<TARGET_FILE:ds_cache>)
add_test(NAME "old_tests::w_r_attr"  COMMAND $<TARGET_FILE:w_r_attr> ${DEV1})
add_test(NAME "old_tests::lock"  COMMAND $<TARGET_FILE:lock> ${DEV1} ${DEV2})
add_test(NAME "old_tests::sub_dev"  COMMAND $<TARGET_FILE:sub_dev> ${DEV1} ${DEV2} ${DEV3})
add_test(NAME "old_tests::print_data"  COMMAND $<TARGET_FILE:print_data> ${DEV1})
add_test(NAME "old_tests::attr_manip"  COMMAND $<TARGET_FILE:attr_manip> ${DEV1})
if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9.2)
    add_test(NAME "old_tests::size"  COMMAND $<TARGET_FILE:size>)
endif()
add_test(NAME "old_tests::ConfEventBugClient"  COMMAND $<TARGET_FILE:ConfEventBugClient> ${DEV3})
add_test(NAME "old_tests::attr_conf_test"  COMMAND $<TARGET_FILE:attr_conf_test> ${DEV1})
