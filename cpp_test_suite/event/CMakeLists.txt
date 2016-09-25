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
            reco_event
            reco_zmq
            server_event
            stateless_sub)

foreach(TEST ${TESTS})
    TEST_SUITE_ADD_TEST(${TEST})
endforeach(TEST)