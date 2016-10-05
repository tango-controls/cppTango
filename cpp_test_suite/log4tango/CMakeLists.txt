set(LOG4TANGO_TEST_SOURCES test_log4tango.cpp)
set(BENCH_TEST_SOURCES clock.cpp clock.hh test_bench.cpp)

add_executable(test_log4tango ${LOG4TANGO_TEST_SOURCES} $<TARGET_OBJECTS:log4tango_objects>)
add_executable(test_bench     ${BENCH_TEST_SOURCES} $<TARGET_OBJECTS:log4tango_objects>)

add_test("log4tango::test"       test_log4tango)
#add_test("log4tango::benchmark"  test_bench)