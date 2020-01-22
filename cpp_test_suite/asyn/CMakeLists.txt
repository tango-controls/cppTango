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

tango_add_test(NAME "asyn::asyn_cmd"  COMMAND $<TARGET_FILE:asyn_cmd> ${DEV1})
tango_add_test(NAME "asyn::asyn_attr"  COMMAND $<TARGET_FILE:asyn_attr> ${DEV1})
tango_add_test(NAME "asyn::asyn_attr_multi"  COMMAND $<TARGET_FILE:asyn_attr_multi> ${DEV1})
tango_add_test(NAME "asyn::asyn_write_attr"  COMMAND $<TARGET_FILE:asyn_write_attr> ${DEV1})
tango_add_test(NAME "asyn::asyn_write_attr_multi"  COMMAND $<TARGET_FILE:asyn_write_attr_multi> ${DEV1})
tango_add_test(NAME "asyn::asyn_cb"  COMMAND $<TARGET_FILE:asyn_cb> ${DEV1} ${DEV2})
tango_add_test(NAME "asyn::asyn_cb2"  COMMAND $<TARGET_FILE:asyn_cb2> ${DEV1} ${DEV2})
tango_add_test(NAME "asyn::asyn_cb_cmd"  COMMAND $<TARGET_FILE:asyn_cb_cmd> ${DEV1})
tango_add_test(NAME "asyn::asyn_attr_cb"  COMMAND $<TARGET_FILE:asyn_attr_cb> ${DEV1})
tango_add_test(NAME "asyn::asyn_write_cb"  COMMAND $<TARGET_FILE:asyn_write_cb> ${DEV1})
tango_add_test(NAME "asyn::auto_asyn_cmd"  COMMAND $<TARGET_FILE:auto_asyn_cmd> ${DEV1})
