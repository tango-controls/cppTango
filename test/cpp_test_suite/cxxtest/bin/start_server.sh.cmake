#!/bin/bash

start_server(){
    echo "Starting DevTest $1"
    @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest $1 -v5 1>@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_$1.out 2>&1 &
    echo $! > @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_$1.pid

    sleep 7
    echo "Done. PID="`cat @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_$1.pid`
}

start_server $1