#TODO windows
message("System TANGO_HOST=$ENV{TANGO_HOST}")

add_custom_target(start-tango ${CMAKE_CURRENT_SOURCE_DIR}/setup.sh
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES setup.sh)

add_custom_target(run-conf_devtest ${CMAKE_CURRENT_SOURCE_DIR}/run.sh ${PROJECT_BINARY_DIR}/cpp_test_suite/new_tests/conf_devtest "${SERV_NAME}/${INST_NAME}" ${DEV1} ${DEV2} ${DEV3} ${DEV1_ALIAS} ${ATTR_ALIAS}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES run.sh)

add_dependencies(run-conf_devtest conf_devtest)

#TODO use starter? systemd?
add_custom_target(start-DevTest ${CMAKE_CURRENT_SOURCE_DIR}/run.sh ${PROJECT_BINARY_DIR}/cpp_test_ds/DevTest ${INST_NAME} -v5 1>/dev/null 2>&1 &
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES run.sh)

add_custom_target(start-FwdTest ${CMAKE_CURRENT_SOURCE_DIR}/run.sh ${PROJECT_BINARY_DIR}/cpp_test_ds/fwd_ds/FwdTest ${INST_NAME} -v5 1>/dev/null 2>&1 &
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES run.sh)

add_custom_target(run-tests ${CMAKE_CURRENT_SOURCE_DIR}/run.sh ${CMAKE_CTEST_COMMAND} -V
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES run.sh)

#TODO stop or kill DevTest and FwdTest

add_custom_target(stop-tango ${CMAKE_CURRENT_SOURCE_DIR}/shutdown.sh
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        SOURCES shutdown.sh)