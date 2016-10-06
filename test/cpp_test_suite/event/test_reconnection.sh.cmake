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
    echo "Starting DevTest $1"
    @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest $1 -v5 1>@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_$1.out 2>&1 &
    echo $! > @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_$1.pid
    echo "Done. PID="`cat @PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_$1.pid`
}

kill_servers(){
    PIDS=`ps -e | grep DevTest | grep -v grep | awk '{print $1}'`
    echo "PIDS = $PIDS"
    for pid in $PIDS
    do
        echo "Killing process with PID $pid"
        kill $pid
    done

    sleep 2
}


start_server "@INST_NAME@2"
sleep 20

echo "Testing server_event"
./server_event @DEV1@ @DEV20@
ret=$?
check_return_value $ret

#
# kill DevTest servers (/10 and /20)
#
kill_servers

#
# Start DevTest server
#

start_server "@INST_NAME@"
sleep 20
#
# Next test will restart server
#

echo "Testing event re-connection (takes time...)"
sleep 18 &&  start_server "@INST_NAME@" &
sleep 62 &&  start_server "@INST_NAME@" &
./reco_zmq @DEV1@ -v
ret=$?
check_return_value $ret

#
# kill servers
#
kill_servers

echo "Testing stateless event connection (takes time...)"
sleep 24 &&  start_server "@INST_NAME@2" &
./stateless_sub @DEV20@ -v
ret=$?
check_return_value $ret

#kill extra server
kill_servers

#restart DEV1
start_server "@INST_NAME@"

sleep 20

echo "PID=$(<@PROJECT_BINARY_DIR@/cpp_test_ds/DevTest_@INST_NAME@.pid)"