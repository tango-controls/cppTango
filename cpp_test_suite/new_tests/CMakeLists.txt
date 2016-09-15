set(TESTS   conf_devtest
            )

foreach(TEST ${TESTS})
    TEST_SUITE_ADD_TEST(${TEST})
endforeach(TEST)