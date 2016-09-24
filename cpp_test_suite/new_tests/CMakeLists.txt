add_executable(conf_devtest conf_devtest.cpp)
target_link_libraries(conf_devtest PUBLIC tango ${CMAKE_DL_LIBS})
add_library(compare_test_object OBJECT compare_test.cpp compare_test.h)

TEST_SUITE_WRAP_EXECUTABLE(conf_devtest)

add_custom_target(run-conf_devtest
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/run_conf_devtest ${SERV_NAME} ${DEV1} ${DEV2} ${DEV3} ${DEV1_ALIAS} ${ATTR_ALIAS})
