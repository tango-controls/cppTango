#!/bin/bash
source tango_host

echo "TANGO_HOST=$TANGO_HOST"

echo "Run conf_devtest"
@PROJECT_BINARY_DIR@/cpp_test_suite/new_tests/conf_devtest @SERV_NAME@/@INST_NAME@ @DEV1@ @DEV2@ @DEV3@ @DEV1_ALIAS@ @ATTR_ALIAS@

echo "Start DevTest"
echo "DevTest libraries:"
echo "`ldd @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest`"
#TODO Starter?
@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest @INST_NAME@ -v6 1>@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.out 2>&1 &
echo $! > @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid

sleep 3

echo "Start FwdTest"
echo "FwdTest libraries:"
echo "`ldd @PROJECT_BINARY_DIR@/cpp_test_ds/fwd_ds/FwdTest`"
@PROJECT_BINARY_DIR@/cpp_test_ds/fwd_ds/FwdTest @INST_NAME@ -v6 1>@PROJECT_BINARY_DIR@/cpp_test_ds/fwd_ds/FwdTest_@INST_NAME@.out 2>&1 &
echo $! > @PROJECT_BINARY_DIR@/cpp_test_ds/fwd_ds/FwdTest_@INST_NAME@.pid

sleep 3