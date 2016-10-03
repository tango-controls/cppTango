#!/usr/bin/env bash
check_return_value () {
    if [ $1 != "0" ]
    then
        echo "Event Test Suite has FAILED !!!!!!!!!!!!!!!!!!"
        echo "Leaving test suite, try to fix it"
        date
        exit -1
    fi
}

start_server(){
    @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest @INST_NAME@ -v5 1>@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.out 2>&1 &
    echo $! > @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid
    echo "PID=" < @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid
}

#
# kill DevTest server
#
PID=$(<@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid)
kill $PID

sleep 2

#
# Start DevTest server
#

start_server
sleep 20
#
# Next test will restart server
#

echo "Testing event re-connection (takes time...)"
sleep 18 &&  start_server &
sleep 62 &&  start_server &
./reco_zmq @DEV1@ -v
ret=$?
check_return_value $ret

#
# kill servers
#

PID=$(<@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid)
kill $PID

sleep 2

#
# Next command will restart server 2
#
PIDS=`ps -e | grep DevTest | grep -v grep`
echo "PIDS = $PIDS"

echo "Testing stateless event connection (takes time...)"
sleep 24 &&  start_server &
./stateless_sub @DEV2@ -v
ret=$?
check_return_value $ret