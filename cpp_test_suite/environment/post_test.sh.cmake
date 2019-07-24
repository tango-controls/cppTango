#!/bin/bash

#TODO Starter?

PID=$(<@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid)
kill $PID

PID=$(<@PROJECT_BINARY_DIR@/cpp_test_ds/fwd_ds/FwdTest_@INST_NAME@.pid)
kill $PID

exit 0
