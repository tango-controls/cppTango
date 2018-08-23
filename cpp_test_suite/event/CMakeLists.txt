set(TESTS   archive_event
            att_conf_event
            back_ch_event
            change_event64
            change_event
            data_ready_event
            event_lock
            multi_event
            pipe_event
#            reco_svc
            scan
            state_event
            user_event
            att_conf_event_buffer
            att_type_event
            back_per_event
            change_event_buffer
            data_ready_event_buffer
            dev_intr_event
            multi_dev_event
            per_event
#            reco_event
            reco_zmq
            server_event
            stateless_sub)

foreach(TEST ${TESTS})
    TEST_SUITE_ADD_TEST(${TEST})
endforeach(TEST)

add_test(NAME "event::per_event" COMMAND $<TARGET_FILE:per_event> ${DEV1} ${DEV2})
add_test(NAME "event::att_type_event" COMMAND $<TARGET_FILE:att_type_event> ${DEV1})
add_test(NAME "event::change_event" COMMAND $<TARGET_FILE:change_event> ${DEV1})
add_test(NAME "event::change_event64" COMMAND $<TARGET_FILE:change_event64> ${DEV1})
add_test(NAME "event::change_event_buffer" COMMAND $<TARGET_FILE:change_event_buffer> ${DEV1})
add_test(NAME "event::archive_event" COMMAND $<TARGET_FILE:archive_event> ${DEV1})
add_test(NAME "event::user_event" COMMAND $<TARGET_FILE:user_event> ${DEV1})
add_test(NAME "event::multi_event" COMMAND $<TARGET_FILE:multi_event> ${DEV1})
add_test(NAME "event::state_event" COMMAND $<TARGET_FILE:state_event> ${DEV1})
add_test(NAME "event::att_conf_event" COMMAND $<TARGET_FILE:att_conf_event> ${DEV1})
add_test(NAME "event::att_conf_event_buffer" COMMAND $<TARGET_FILE:att_conf_event_buffer> ${DEV1})
add_test(NAME "event::data_ready_event" COMMAND $<TARGET_FILE:data_ready_event> ${DEV1})
add_test(NAME "event::data_ready_event_buffer" COMMAND $<TARGET_FILE:data_ready_event_buffer> ${DEV1})
add_test(NAME "event::multi_dev_event" COMMAND $<TARGET_FILE:multi_dev_event> ${DEV1} ${DEV2} ${DEV3})
add_test(NAME "event::dev_intr_event" COMMAND $<TARGET_FILE:dev_intr_event> ${DEV1})
add_test(NAME "event::pipe_event" COMMAND $<TARGET_FILE:pipe_event> ${DEV1})
add_test(NAME "event::event_lock" COMMAND $<TARGET_FILE:event_lock> ${DEV1})


#the following test performs several kill/start sequences, but must keep DEV1 alive in the end
configure_file(test_reconnection.sh.cmake test_reconnection.sh @ONLY)
execute_process(COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/test_reconnection.sh)
add_test(NAME "event::reconnect" COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_reconnection.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})