set(TESTS   conf_devtest
            )

foreach(TEST ${TESTS})
    TEST_SUITE_ADD_TEST(${TEST})
endforeach(TEST)

add_library(compare_test_object OBJECT compare_test.cpp compare_test.h)