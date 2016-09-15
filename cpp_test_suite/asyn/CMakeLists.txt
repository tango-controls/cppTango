set(TESTS   asyn_attr_cb
            asyn_attr
            asyn_attr_multi
            asyn_cb2
            asyn_cb_cmd
            asyn_cb
            asyn_cmd
            asyn_faf
            asyn_thread
            asyn_write_attr
            asyn_write_attr_multi
            asyn_write_cb
            auto_asyn_cmd)

foreach(TEST ${TESTS})
    TEST_SUITE_ADD_TEST(${TEST})
endforeach(TEST)